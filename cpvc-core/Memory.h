#pragma once

#include <map>
#include <memory>
#include <array>
#include "common.h"
#include "StreamReader.h"
#include "StreamWriter.h"

typedef std::array<byte, 0x4000> Mem16k;

inline Mem16k CreateMem16k(byte* pBuffer)
{
    Mem16k mem;
    memcpy(mem.data(), pBuffer, mem.size());

    return mem;
}

struct CoreSnapshot;
struct SnapshotZ80Mem;
struct Snapshot2nd64kAndRoms;

class Memory
{
public:
    Memory()
    {
        Reset();

        _lowerRom.fill(0);
        _upperRom.fill(0);
    };

    ~Memory() {};

    void CopyFrom(const Memory& memory, bool skipBanks = false)
    {
        for (int i = 0; i < 8; i++)
        {
            _banks[i] = memory._banks[i];
        }

        _ramConfig = memory._ramConfig;
        _lowerRomEnabled = memory._lowerRomEnabled;
        _lowerRom = memory._lowerRom;
        _upperRomEnabled = memory._upperRomEnabled;
        _upperRom = memory._upperRom;
        _selectedUpperRom = memory._selectedUpperRom;

        _roms.clear();
        for (std::pair<byte, Mem16k> x : memory._roms)
        {
            _roms[x.first] = x.second;
        }

        _romIds.clear();
        for (std::pair<byte, int> x : memory._romIds)
        {
            _romIds[x.first] = x.second;
        }

        ConfigureRAM();
    }

    void SaveTo(SnapshotZ80Mem& snapshot);
    void LoadFrom(SnapshotZ80Mem& snapshot);

    void SaveTo(Snapshot2nd64kAndRoms& snapshot);
    void LoadFrom(Snapshot2nd64kAndRoms& snapshot);

    // ROM cache
    static int _nextRomId;
    static std::map<int, Mem16k> _romCache;
    static int GetRomId(const Mem16k& rom);
    static bool GetRom(int id, Mem16k& rom);

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
    int _lowerRomId;

    bool _upperRomEnabled;
    Mem16k _upperRom;
    int _upperRomId;
    byte _selectedUpperRom;
    std::map<byte, Mem16k> _roms;
    std::map<byte, int> _romIds;

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

    void SetUpperROM(byte slot, const Mem16k& rom)
    {
        _romIds[slot] = GetRomId(rom);
        _roms[slot] = rom;
        if (_selectedUpperRom == slot)
        {
            _upperRom = _roms[_selectedUpperRom];
        }
    }

    void RemoveUpperROM(byte slot)
    {
        _romIds.erase(slot);
        _roms.erase(slot);
    }

    void EnableUpperROM(bool enable)
    {
        _upperRomEnabled = enable;
        ConfigureRAM();
    }

    void SelectROM(byte rom)
    {
        if (_roms.find(rom) == _roms.end())
        {
            _selectedUpperRom = 0;
            if (_roms.find(0) == _roms.end())
            {
                _upperRom.fill(0);
            }
        }
        else
        {
            _selectedUpperRom = rom;
            _upperRom = _roms[rom];
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
        s << memory._roms;

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
        s >> memory._roms;


        // Cache roms...
        memory._lowerRomId = Memory::GetRomId(memory._lowerRom);

        memory._romIds.clear();
        for (std::pair<byte, Mem16k> i : memory._roms)
        {
            memory._romIds[i.first] = Memory::GetRomId(memory._roms[i.first]);
        }


        // Probably more consistent to serialize each read and write bank separately, as it's not
        // guaranteed that they will be in sync with _ramConfig, even though they should be!
        memory.ConfigureRAM();

        // Ensure the upper rom is copied to _upperRom...
        memory.SelectROM(memory._selectedUpperRom);

        return s;
    }
};

