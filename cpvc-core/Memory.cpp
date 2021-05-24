#include "Memory.h"

RomId Memory::_nextRomId = (emptyRomId + 1);
std::map<RomId, Mem16k> Memory::_romCache;
const Mem16k Memory::emptyRom = {};


int Memory::GetRomId(const Mem16k& rom)
{
    for (std::pair<RomId, Mem16k> i : _romCache)
    {
        if (memcmp(&i.second[0], &rom[0], 0x4000) == 0)
        {
            return i.first;
        }
    }

    RomId romId = _nextRomId;
    _nextRomId++;
    _romCache[romId] = rom;

    return romId;
}
