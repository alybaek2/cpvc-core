#include "Memory.h"
#include "CoreSnapshot.h"

int Memory::_nextRomId = 0;
std::map<int, Mem16k> Memory::_romCache;

void Memory::SaveTo(SnapshotZ80Mem& snapshot)
{
    snapshot._lowerRomEnabled = _lowerRomEnabled;
    snapshot._upperRomEnabled = _upperRomEnabled;
    snapshot._ramConfig = _ramConfig;
    snapshot._selectedUpperRom = _selectedUpperRom;

    for (byte b = 0; b < 4; b++)
    {
        memcpy(&snapshot._banks[b][0], &_banks[b][0], 0x4000);
    }
}

void Memory::LoadFrom(SnapshotZ80Mem& snapshot)
{
    _lowerRomEnabled = snapshot._lowerRomEnabled;
    _upperRomEnabled = snapshot._upperRomEnabled;
    _ramConfig = snapshot._ramConfig;
    _selectedUpperRom = snapshot._selectedUpperRom;

    for (byte b = 0; b < 4; b++)
    {
        memcpy(&_banks[b][0], &snapshot._banks[b][0], 0x4000);
    }

    ConfigureRAM();
}

void Memory::SaveTo(Snapshot2nd64kAndRoms& snapshot)
{
    bool allZeroes = true;
    for (byte b = 5; b < 8; b++)
    {
        for (word w = 0; w < 0x4000; w++)
        {
            if (_banks[b][w] != 0)
            {
                allZeroes = true;
                break;
            }
        }
    }

    if (allZeroes)
    {
        snapshot._allZeroes = true;
    }
    else
    {
        for (byte b = 5; b < 8; b++)
        {
            snapshot._banks[b].resize(0x4000);
            memcpy(snapshot._banks[b].data(), &_banks[b][0], 0x4000);
        }
    }

    // Roms!
    snapshot._lowerRomId = _lowerRomId;

    snapshot._upperRomIds.clear();
    for (std::pair<byte, int> i : _romIds)
    {
        snapshot._upperRomIds[i.first] = i.second;
    }
}

void Memory::LoadFrom(Snapshot2nd64kAndRoms& snapshot)
{
    if (snapshot._allZeroes)
    {
        for (byte b = 5; b < 8; b++)
        {
            memset(&_banks[b][0], 0, 0x4000);
        }
    }
    else
    {
        for (byte b = 5; b < 8; b++)
        {
            memcpy(&_banks[b][0], &snapshot._banks[b][0], 0x4000);
        }
    }

    // Roms!
    _lowerRomId = snapshot._lowerRomId;
    GetRom(_lowerRomId, _lowerRom);

    _roms.clear();
    _romIds.clear();
    for (std::pair<byte, int> i : snapshot._upperRomIds)
    {
        _romIds[i.first] = i.second;
        _roms[i.first] = Mem16k();
        GetRom(i.second, _roms[i.first]);
    }

    GetRom(_romIds[_selectedUpperRom], _upperRom);
}

int Memory::GetRomId(const Mem16k& rom)
{
    for (std::pair<int, Mem16k> i : _romCache)
    {
        if (memcmp(&i.second[0], &rom[0], 0x4000) == 0)
        {
            return i.first;
        }
    }

    int romId = _nextRomId;
    _nextRomId++;
    _romCache[romId] = rom;

    return romId;
}

bool Memory::GetRom(int id, Mem16k& rom)
{
    std::map<int, Mem16k>::iterator i = _romCache.find(id);
    if (i != _romCache.end())
    {
        rom = i->second;
        return true;
    }

    return false;
}
