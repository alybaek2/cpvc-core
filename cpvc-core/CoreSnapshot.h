
#pragma once

#include "common.h"

#include "Memory.h"
#include "PPI.h"
#include "PSG.h"
#include "Keyboard.h"
#include "GateArray.h"
#include "Tape.h"
#include "FDC.h"
#include "CRTC.h"

struct Mem16kSnapshot
{
    bool _allZeroes;
    std::vector<byte> _bytes;
};

struct SnapshotZ80Mem
{
    word AF;
    word BC;
    word DE;
    word HL;
    word IR;

    word AF_;
    word BC_;
    word DE_;
    word HL_;

    word IX;
    word IY;

    word PC;
    word SP;

    bool _iff1;
    bool _iff2;
    bool _interruptRequested;
    byte _interruptMode;
    byte _eiDelay;
    bool _halted;

    bool _lowerRomEnabled;
    bool _upperRomEnabled;
    byte _selectedUpperRom;
    byte _ramConfig;

    Keyboard _keyboard;

    // Gate Array stuff.
    byte _selectedPen;
    byte _pen[16];
    byte _border;
    byte _mode;

    // PPI
    bool _printerReady;
    bool _exp;
    bool _refreshRate;
    byte _manufacturer;

    bool _tapeWriteData;

    byte _portA;
    byte _portB;
    byte _portC;
    byte _control;

    // PSG
    bool _bdir;
    bool _bc1;
    byte _selectedRegister;
    byte _register[16];

    word _toneTicks[3];
    bool _state[3];

    word _noiseTicks;
    bool _noiseAmplitude;
    word _noiseRandom;

    word _envelopeTickCounter;
    byte _envelopeStepCount;
    word _envelopePeriodCount;
    byte _envelopeState;
    word _noiseTickCounter;
    byte _envelopeStepState;

    // CRTC
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

    byte _crtcRegister[18];

    byte _crtcSelectedRegister;

    // First 64k of memory.
    Mem16k _banks[4];
};

struct Snapshot2nd64kAndRoms
{
    bool _allZeroes;
    bytevector _banks[4];
    //Mem16k _banks[4];
    int _lowerRomId;
    std::map<byte, int> _upperRomIds;
};

template <typename S>
class SubSnapshot
{
public:
    SubSnapshot(std::shared_ptr<S> parent = nullptr)
    {
        _full = std::make_shared<S>();
        _diffParent = parent;
    };

    ~SubSnapshot() {};

    std::shared_ptr<S> Full()
    {
        return _full;
    }

private:
    std::shared_ptr<S> _full;

    bytevector _diff;
    std::shared_ptr<S> _diffParent;
};

// Optimzed core storage.
struct CoreSnapshot
{
public:
    int parentSnapshotId;  // -1 if no parent.

    std::shared_ptr<CoreSnapshot> pParentSnapshot;

    // Serializable stuff that can be stored as a full image or a compressed diff.
    SubSnapshot<SnapshotZ80Mem> z80MemStuff;
    SubSnapshot<Snapshot2nd64kAndRoms> mem2nd64kStuff;

    // Stuff that is not yet serializable...

    bytevector z80MemDiff;
    
    Snapshot2nd64kAndRoms mem2nd64k;

    FDC _fdc;
    Keyboard _keyboard;
    CRTC _crtc = CRTC((bool&)(*(bool*)nullptr));
    PSG _psg = PSG(_keyboard);
    PPI _ppi = PPI(_psg, _keyboard, &_crtc._inVSync, &_tape._motor, &_tape._level);
    GateArray _gateArray = GateArray((Memory&)(*(Memory*)nullptr), (bool&)(*(bool*)nullptr), _crtc._scanLineCount);
    Tape _tape;

    // The CPC's internal "clock"; each tick represents 0.25 microseconds.
    qword _ticks;

    dword _frequency = 48000;
    dword _audioTickTotal;
    dword _audioTicksToNextSample;
    dword _audioSampleCount;

    word _scrHeight;
    word _scrWidth;
    word _scrPitch;
    bytevector _screen;
};

