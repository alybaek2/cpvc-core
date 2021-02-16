#pragma once

#include <memory>
#include <vector>

typedef uint8_t byte;
typedef int8_t offset;
typedef uint16_t word;
typedef uint32_t dword;
typedef uint64_t qword;

typedef std::vector<byte> bytevector;
typedef std::vector<word> wordvector;

inline bool Bit(word value, byte bit) { return ((value & (1 << bit)) != 0); }

inline byte LowNibble(byte b) { return b & 0x0F; }
inline byte HighNibble(byte b) { return b & 0xF0; }

inline word MakeWord(byte high, byte low) { return ((word)((high << 8) | low)); }

inline byte& Low(word& w) { return *((byte*)&w); }
inline byte& High(word& w) { return *(((byte*)&w) + 1); }

inline const byte& Low(const word& w) { return *(((byte*)&w) + 0); }
inline const byte& High(const word& w) { return *(((byte*)&w) + 1); }

inline const word& Low(const dword& dw) { return *(((word*)&dw) + 0); }
inline const word& High(const dword& dw) { return *(((word*)&dw) + 1); }
