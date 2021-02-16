#pragma once

#include <sstream>
#include <iomanip>

#include "common.h"

template <int S>
std::string StringifyByteArray(const byte(&t)[S])
{
    std::ostringstream ss;
    for (byte b : t)
    {
        ss << std::setw(2) << std::hex << (int)b;
    }

    return ss.str();
}

template <int S>
std::string StringifyByteArray(const std::array<byte, S>(&t))
{
    std::ostringstream ss;
    for (byte b : t)
    {
        ss << std::setw(2) << std::hex << (int)b;
    }

    return ss.str();
}

__inline std::string StringifyByteArray(const bytevector& t)
{
    std::ostringstream ss;
    for (byte b : t)
    {
        ss << std::setw(2) << std::hex << (int)b;
    }

    return ss.str();
}

