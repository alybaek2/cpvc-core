#pragma once

#include "common.h"
#include "Blob.h"

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
public:
    bool _allZeroes;
    std::vector<byte> _bytes;
};

struct SnapshotZ80Mem
{
public:
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

    // First 64k of memory.
    Mem16k _banks[4];
};

struct Snapshot2nd64kAndRoms
{
public:
    bool _allZeroes;
    bytevector _banks[4];
    int _lowerRomId;
    std::map<byte, int> _upperRomIds;
};

struct SnapshotSimpleHardware
{
public:
    Keyboard _keyboard;
    CRTC _crtc;
    PSG _psg;
    PPI _ppi;
    GateArray _gateArray;

    qword _ticks;

    dword _frequency;
    dword _audioTickTotal;
    dword _audioTicksToNextSample;
    dword _audioSampleCount;

    word _scrHeight;
    word _scrWidth;
    word _scrPitch;
};

// Optimized core storage.
struct CoreSnapshot
{
public:
    // Serializable stuff that can be stored as a full image or a compressed diff.
    std::shared_ptr<Blob<SnapshotZ80Mem>> _z80MemStuff;
    std::shared_ptr<Blob<byte>> _screenBlob;
    std::shared_ptr<Blob<SnapshotSimpleHardware>> _simpleHardware;

    // Stuff that is not yet serializable...
    Snapshot2nd64kAndRoms mem2nd64k;
    FDC _fdc;
    Tape _tape;
};

