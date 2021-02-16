#pragma once

#include "common.h"
#include <sstream>

#include "Memory.h"
#include "IBus.h"


class GateArray : public IBusNoAddressWriteOnly
{
public:
    GateArray(Memory& memory, bool& interruptRequested, byte& scanLineCount);
    ~GateArray();

    void CopyFrom(const GateArray& gateArray)
    {
        _selectedPen = gateArray._selectedPen;
        memcpy(_pen, gateArray._pen, sizeof(_pen));
        _border = gateArray._border;
        _mode = gateArray._mode;
        memcpy(_renderedPenBytes, gateArray._renderedPenBytes, sizeof(_renderedPenBytes));
    }

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

    friend std::ostream& operator<<(std::ostream& s, const GateArray& gateArray);
};