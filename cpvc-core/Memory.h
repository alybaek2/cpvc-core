#pragma once

#include <array>
#include <map>
#include <memory>

#include "common.h"
#include "Rom.h"
#include "stringify.h"
#include "Serialize.h"
#include "StreamReader.h"
#include "StreamWriter.h"

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
        Reset();

        _lowerRom = _emptyRom;
    };

    ~Memory() {};

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
    bool _upperRomEnabled;
    RomSlot _selectedUpperRom;

    Rom _lowerRom;
    std::map<RomSlot, Rom> _upperRoms;

    static Rom _emptyRom;

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
        _lowerRom = Rom(lowerRom);
    }

    void EnableLowerROM(bool enable)
    {
        _lowerRomEnabled = enable;
        ConfigureRAM();
    }

    void SetUpperROM(RomSlot slot, const Mem16k& rom)
    {
        _upperRoms[slot] = rom;
        ConfigureRAM();
    }

    void RemoveUpperROM(RomSlot slot)
    {
        _upperRoms.erase(slot);
        ConfigureRAM();
    }

    void EnableUpperROM(bool enable)
    {
        _upperRomEnabled = enable;
        ConfigureRAM();
    }

    void SelectROM(RomSlot slot)
    {
        _selectedUpperRom = slot;
        ConfigureRAM();
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
            _readRAM[0] = _lowerRom.Data();
        }

        if (_upperRomEnabled)
        {
            RomSlot selectedUpperRom = _selectedUpperRom;
            if (_upperRoms.find(_selectedUpperRom) == _upperRoms.end())
            {
                selectedUpperRom = 0;
            }

            Rom upperRom = (_upperRoms.find(selectedUpperRom) == _upperRoms.end()) ? _emptyRom : _upperRoms[selectedUpperRom];
            _readRAM[3] = upperRom.Data();
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
        s << memory._upperRoms;

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
        s >> memory._upperRoms;

        // Probably more consistent to serialize each read and write bank separately, as it's not
        // guaranteed that they will be in sync with _ramConfig, even though they should be!
        memory.ConfigureRAM();

        return s;
    }

    friend std::ostringstream& operator<<(std::ostringstream& s, const Memory& memory)
    {
        for (const Mem16k bank : memory._banks)
        {
            s << "Memory (16k): " << StringifyByteArray(bank) << std::endl;
        }

        s << memory._banks;

        s << (int)memory._ramConfig << std::endl;
        s << memory._lowerRomEnabled << std::endl;
        s << memory._upperRomEnabled << std::endl;
        s << (int)memory._selectedUpperRom << std::endl;
        s << "Lower rom: " << StringifyByteArray(memory._lowerRom.Image()) << std::endl;
        
        for (const std::pair<RomSlot, Rom>& kv : memory._upperRoms)
        {
            s << "Upper rom " << (int)kv.first << ":" << StringifyByteArray(kv.second.Image()) << std::endl;
        }

        return s;
    }

    SERIALIZE_MEMBERS_WITH_POSTREAD(
        _banks,
        _ramConfig,
        _lowerRomEnabled,
        _upperRomEnabled,
        _selectedUpperRom,
        _lowerRom,
        _upperRoms);

    void SerializePostRead()
    {
        ConfigureRAM();
    }
};
