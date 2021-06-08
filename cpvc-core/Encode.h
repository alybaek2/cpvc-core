#pragma once

#include "common.h"

bool RunLengthDecode(const bytevector& encodedBytes, bytevector& decoded);
bool RunLengthEncode(byte* pBuffer, size_t size, bytevector& encoded);

