#include "Rom.h"

RomId Rom::_nextRomId = 1;
std::map<RomId, Mem16k> Rom::_romCache;
