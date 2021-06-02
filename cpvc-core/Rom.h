#pragma once

#include <map>

#include "common.h"
#include "Serialize.h"
#include "StreamReader.h"
#include "StreamWriter.h"

using RomId = int;
using RomSlot = byte;

class Rom
{
public:
    Rom() : Rom(Mem16k())
    {
    }

    Rom(RomId id)
    {
        _romId = id;
    }

    Rom(const Mem16k& image)
    {
        _romId = GetRomId(image);
    }

    friend StreamWriter& operator<<(StreamWriter& s, const Rom& rom)
    {
        s << rom.Image();

        return s;
    }

    friend StreamReader& operator>>(StreamReader& s, Rom& rom)
    {
        Mem16k image;
        s >> image;

        rom._romId = GetRomId(image);

        return s;
    }

    SERIALIZE_MEMBERS(_romId);

    const Mem16k& Image() const
    {
        return _romCache[_romId];
    }

    static int GetRomId(const Mem16k& rom)
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

private:
    RomId _romId;

    static int _nextRomId;
    static std::map<int, Mem16k> _romCache;
};
