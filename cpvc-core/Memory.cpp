#include "Memory.h"
#include "CoreSnapshot.h"

int Memory::_nextRomId = 0;
std::map<int, Mem16k> Memory::_romCache;

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
