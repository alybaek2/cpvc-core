#pragma once

#include <map>
#include <memory>
#include <array>
#include "common.h"
#include "stringify.h"
#include "StreamReader.h"
#include "StreamWriter.h"

#include "Serialize.h"

using RomId = int;
using RomSlot = byte;
using Mem16k = std::array<byte, 0x4000>;

inline Mem16k CreateMem16k(byte* pBuffer)
{
    Mem16k mem;
    memcpy(mem.data(), pBuffer, mem.size());

    return mem;
}

class Memory
{
public:
    Memory()
    {
        for (word r = 0; r < 256; r++)
        {
            _romIds[r] = emptyRomId;
        }

        Reset();

        _lowerRom = emptyRom;
        _upperRom = emptyRom;
    };

    ~Memory() {};

    // ROM cache
    static int GetRomId(const Mem16k& rom);
    //static bool GetRom(int id, Mem16k& rom);

private:
    Mem16k _banks[8];
    byte* _readRAM[4];
    byte* _writeRAM[4];

    byte _ramConfig;
    const byte _ramConfigs[8][4] = {
        { 0, 1, 2, 3 },
        { 0, 1, 2, 7 },
        { 4, 5, 6, 7 },
        { 0, 3, 2, 7 },
        { 0, 4, 2, 3 },
        { 0, 5, 2, 3 },
        { 0, 6, 2, 3 },
        { 0, 7, 2, 3 }
    };

    bool _lowerRomEnabled;
    Mem16k _lowerRom;
    RomId _lowerRomId;

    bool _upperRomEnabled;
    Mem16k _upperRom;
    RomId _upperRomId;
    byte _selectedUpperRom;
    RomId _romIds[256];

    static int _nextRomId;
    static std::map<int, Mem16k> _romCache;

    static constexpr RomId emptyRomId = 0;
    static const Mem16k emptyRom;

public:
    void Reset()
    {
        for (Mem16k& mem : _banks)
        {
            mem.fill(0);
        }

        _lowerRomEnabled = true;
        _upperRomEnabled = true;
        SelectROM(0);

        SetRAMConfig(0);
    }

    byte VideoRead(const word& addr)
    {
        byte bankIndex = addr >> 14;
        return _writeRAM[bankIndex][addr & 0x3FFF];
    }

    byte Read(const word& addr)
    {
        byte bankIndex = addr >> 14;
        return _readRAM[bankIndex][addr & 0x3FFF];
    }

    void Write(const word& addr, byte b)
    {
        byte bankIndex = addr >> 14;
        _writeRAM[bankIndex][addr & 0x3FFF] = b;
    }

    void SetLowerROM(const Mem16k& lowerRom)
    {
        _lowerRomId = GetRomId(lowerRom);
        _lowerRom = lowerRom;
    }

    void EnableLowerROM(bool enable)
    {
        _lowerRomEnabled = enable;
        ConfigureRAM();
    }

    void SetUpperROM(RomSlot slot, const Mem16k& rom)
    {
        _romIds[slot] = GetRomId(rom);
        if (_selectedUpperRom == slot)
        {
            _upperRom = rom;
        }
    }

    void RemoveUpperROM(RomSlot slot)
    {
        _romIds[slot] = emptyRomId;
    }

    void EnableUpperROM(bool enable)
    {
        _upperRomEnabled = enable;
        ConfigureRAM();
    }

    void SelectROM(RomSlot slot)
    {
        RomId id = _romIds[slot];
        if (id == emptyRomId)
        {
            // Should we just leave _selectedUpperRom as it is in this case?
            _selectedUpperRom = 0;

            if (_romIds[_selectedUpperRom] == emptyRomId)
            {
                _upperRom = emptyRom;
            }
        }
        else
        {
            _selectedUpperRom = slot;
            _upperRom = _romCache[id];
        }
    }

    void SetRAMConfig(byte config)
    {
        _ramConfig = config & 0x07;
        ConfigureRAM();
    }

    void ConfigureRAM()
    {
        for (byte b = 0; b < 4; b++)
        {
            _readRAM[b] = _writeRAM[b] = _banks[_ramConfigs[_ramConfig][b]].data();
        }

        if (_lowerRomEnabled)
        {
            _readRAM[0] = _lowerRom.data();
        }

        if (_upperRomEnabled)
        {
            _readRAM[3] = _upperRom.data();
        }
    }

    friend StreamWriter& operator<<(StreamWriter& s, const Memory& memory)
    {
        s << memory._banks;
        s << memory._ramConfig;
        s << memory._lowerRomEnabled;
        s << memory._upperRomEnabled;
        s << memory._selectedUpperRom;
        s << memory._lowerRom;

        std::map<RomSlot, Mem16k> roms;
        for (word i = 0; i < 256; i++)
        {
            RomSlot slot = static_cast<RomSlot>(i);
            RomId romId = memory._romIds[slot];
            if (romId != emptyRomId)
            {
                roms[slot] = Memory::_romCache[romId];
            }
        }

        s << roms;

        return s;
    }

    friend StreamReader& operator>>(StreamReader& s, Memory& memory)
    {
        s >> memory._banks;
        s >> memory._ramConfig;
        s >> memory._lowerRomEnabled;
        s >> memory._upperRomEnabled;
        s >> memory._selectedUpperRom;
        s >> memory._lowerRom;

        std::map<RomSlot, Mem16k> roms;
        s >> roms;

        for (word i = 0; i < 256; i++)
        {
            RomSlot slot = static_cast<RomSlot>(i);
            memory._romIds[slot] = emptyRomId;
        }

        for (const std::pair<RomSlot, Mem16k>& kv : roms)
        {
            memory._romIds[kv.first] = memory.GetRomId(kv.second);
        }

        // Cache roms...
        memory._lowerRomId = Memory::GetRomId(memory._lowerRom);

        // Probably more consistent to serialize each read and write bank separately, as it's not
        // guaranteed that they will be in sync with _ramConfig, even though they should be!
        memory.ConfigureRAM();

        // Ensure the upper rom is copied to _upperRom...
        memory.SelectROM(memory._selectedUpperRom);

        return s;
    }

    friend std::ostringstream& operator<<(std::ostringstream& s, const Memory& memory)
    {
        for (const Mem16k bank : memory._banks)
        {
            s << "Memory (16k): " << StringifyByteArray(memory._lowerRom) << std::endl;
        }

        s << memory._banks;

        s << (int)memory._ramConfig << std::endl;
        s << memory._lowerRomEnabled << std::endl;
        s << memory._upperRomEnabled << std::endl;
        s << (int)memory._selectedUpperRom << std::endl;
        s << "Lower rom: " << StringifyByteArray(memory._lowerRom) << std::endl;
        
        return s;
    }

    SERIALIZE_MEMBERS_WITH_POSTREAD(
        _banks,
        _ramConfig,
        _lowerRomEnabled,
        _upperRomEnabled,
        _selectedUpperRom,
        _lowerRomId,
        _romIds);

    void SerializePostRead()
    {
        _lowerRom = _romCache[_lowerRomId];
        _upperRom = _romCache[_romIds[_selectedUpperRom]];

        ConfigureRAM();
    }
};
