#pragma once

#include "common.h"
#include <sstream>

#include "Memory.h"
#include "IBus.h"

#include "Serialize.h"

class GateArray : public IBusNoAddressWriteOnly
{
public:
    GateArray(Memory& memory, bool& interruptRequested, byte& scanLineCount);
    ~GateArray();

    Memory& _memory;
    byte _selectedPen;
    byte _pen[16];
    byte _border;
    byte _mode;

    byte& _scanLineCount;
    bool& _interruptRequested;

    byte _renderedPenBytes[4][256][8];

    void Reset();

    void Write(byte b);

    void RenderPens();

    friend StreamWriter& operator<<(StreamWriter& s, const GateArray& gateArray);
    friend StreamReader& operator>>(StreamReader& s, GateArray& gateArray);

    SERIALIZE_MEMBERS(
        _selectedPen,
        _pen,
        _border,
        _mode)

    friend std::ostringstream& operator<<(std::ostringstream& s, const GateArray& gateArray);
};
