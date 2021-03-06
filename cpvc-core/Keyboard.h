#pragma once

#include "common.h"
#include "stringify.h"
#include "StreamReader.h"
#include "StreamWriter.h"
#include "Serialize.h"

class Keyboard
{
public:
    Keyboard();
    ~Keyboard();

    void Reset();
    bool KeyPress(byte line, byte bit, bool down);
    byte ReadSelectedLine();

    void SelectLine(byte line);
    byte SelectedLine();

    SERIALIZE_MEMBERS(
        _matrix,
        _matrixClash,
        _selectedLine)

private:
    constexpr static byte _lineCount = 10;
    byte _matrix[_lineCount];
    byte _matrixClash[_lineCount];
    byte _selectedLine;

    void Clash();
    byte SetLineState(byte(&matrix)[_lineCount], byte line, byte bit, bool state);

    friend StreamWriter& operator<<(StreamWriter& s, const Keyboard& keyboard);
    friend StreamReader& operator>>(StreamReader& s, Keyboard& keyboard);

    friend std::ostream& operator<<(std::ostream& s, const Keyboard& keyboard);
    friend std::ostringstream& operator<<(std::ostringstream& s, const Keyboard& keyboard);
};
