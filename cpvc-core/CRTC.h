#pragma once

#include "common.h"

#include "IBus.h"
#include "StreamReader.h"
#include "StreamWriter.h"

class CRTC : public IBus
{
public:
    CRTC(bool& requestInterrupt);
    ~CRTC();

    void CopyFrom(const CRTC& crtc)
    {
        _x = crtc._x;
        _y = crtc._y;
        _hCount = crtc._hCount;
        _vCount = crtc._vCount;
        _raster = crtc._raster;
        _inHSync = crtc._inHSync;
        _hSyncCount = crtc._hSyncCount;
        _inVSync = crtc._inVSync;
        _vSyncCount = crtc._vSyncCount;
        _inVTotalAdjust = crtc._inVTotalAdjust;
        _vTotalAdjustCount = crtc._vTotalAdjustCount;

        _scanLineCount = crtc._scanLineCount;
        _vSyncDelay = crtc._vSyncDelay;

        _memoryAddress = crtc._memoryAddress;

        memcpy(_register, crtc._register, sizeof(_register));

        _selectedRegister = crtc._selectedRegister;
    }

    void Tick();

    byte Read(word addr);
    void Write(word addr, byte b);

    void Reset();

    // Set the top of the screen to be a little above the top of the screen buffer; this helps align the
    // screen better and can be thought of as emulating CRT overscan.
    constexpr static word _yTop = -16;

    byte _x;
    word _y;
    byte _hCount;
    byte _vCount;
    byte _raster;
    bool _inHSync;
    byte _hSyncCount;
    bool _inVSync;
    byte _vSyncCount;
    bool _inVTotalAdjust;
    byte _vTotalAdjustCount;

    byte _scanLineCount;
    byte _vSyncDelay;

    word _memoryAddress;

    byte _register[18];

    byte& _horizontalTotal = _register[0];
    byte& _horizontalDisplayed = _register[1];
    byte& _horizontalSyncPosition = _register[2];
    byte& _horizontalAndVerticalSyncWidth = _register[3];
    byte& _verticalTotal = _register[4];
    byte& _verticalTotalAdjust = _register[5];
    byte& _verticalDisplay = _register[6];
    byte& _verticalSyncPosition = _register[7];
    byte& _interlaceAndSkew = _register[8];
    byte& _maximumRasterAddress = _register[9];
    byte& _cursorStartRaster = _register[10];
    byte& _cursorEndRaster = _register[11];
    byte& _displayStartAddressHigh = _register[12];
    byte& _displayStartAddressLow = _register[13];
    byte& _cursorAddressHigh = _register[14];
    byte& _cursorAddressLow = _register[15];
    byte& _lightPenAddressHigh = _register[16];
    byte& _lightPenAddressLow = _register[17];

private:
    byte _selectedRegister;

    bool& _requestInterrupt;

    byte ReadRegister();
    void WriteRegister(byte b);

    void VSyncStart();
    void HSyncEnd();

    friend StreamWriter& operator<<(StreamWriter& s, const CRTC& crtc);
    friend StreamReader& operator>>(StreamReader& s, CRTC& crtc);
};