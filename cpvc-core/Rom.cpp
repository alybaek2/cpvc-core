#include "Rom.h"

RomId Rom::_nextRomId = 0;
std::map<RomId, Mem16k> Rom::_romCache;
