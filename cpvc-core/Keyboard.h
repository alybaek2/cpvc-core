#pragma once

#include "common.h"
#include "StreamReader.h"
#include "StreamWriter.h"

class Keyboard
{
public:
    Keyboard();
    ~Keyboard();

    void CopyFrom(const Keyboard& keyboard)
    {
        memcpy(_matrix, keyboard._matrix, sizeof(_matrix));
        memcpy(_matrixClash, keyboard._matrixClash, sizeof(_matrixClash));
        _selectedLine = keyboard._selectedLine;
    }

    void Reset();
    bool KeyPress(byte line, byte bit, bool down);
    byte ReadSelectedLine();

    void SelectLine(byte line);
    byte SelectedLine();

private:
    constexpr static byte _lineCount = 10;
    byte _matrix[_lineCount];
    byte _matrixClash[_lineCount];
    byte _selectedLine;

    void Clash();
    byte SetLineState(byte(&matrix)[_lineCount], byte line, byte bit, bool state);

    friend StreamWriter& operator<<(StreamWriter& s, const Keyboard& keyboard);
    friend StreamReader& operator>>(StreamReader& s, Keyboard& keyboard);
};

