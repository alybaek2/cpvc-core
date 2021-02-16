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

    // Floppy controller stuff
    byte _readBuffer[readBufferSize];
    byte _readBufferIndex;
    signed char _readTimeout;

    byte _mainStatus;
    byte _data;
    byte _dataDirection;
    byte _motor;
    byte _currentDrive;
    byte _currentHead;
    byte _status[4];

    bool _seekCompleted[2];
    bool _statusChanged[2];

    int _phase;
    byte _commandBytes[100];
    byte _commandByteCount;
    byte _execBytes[1024];
    word _execByteCount;
    word _execIndex;
    byte _resultBytes[100];
    byte _resultByteCount;
    byte _resultIndex;

    byte _stepReadTime;
    byte _headLoadTime;
    byte _headUnloadTime;
    byte _nonDmaMode;

    // Drive stuff...
    byte _currentSector0;
    size_t _currentTrack0;
    bool _hasDisk0;

    byte _currentSector1;
    size_t _currentTrack1;
    bool _hasDisk1;
};

struct Snapshot2nd64kAndRoms
{
public:
    bool _allZeroes;
    bytevector _banks[4];
    int _lowerRomId;
    std::map<byte, int> _upperRomIds;
};

// Optimized core storage.
struct CoreSnapshot
{
public:
    CoreSnapshot()
    {
        _z80MemStuff = std::make_shared<Blob<SnapshotZ80Mem>>();
        _screenBlob = std::make_shared<Blob<byte>>();
        _floppyAImage = std::make_shared<Blob<byte>>();
        _floppyBImage = std::make_shared<Blob<byte>>();
        _tapeImage = std::make_shared<Blob<byte>>();
    }

    std::shared_ptr<CoreSnapshot> SetDiffParent(std::shared_ptr<CoreSnapshot>& parentSnapshot)
    {
        std::unique_ptr<bytevector> tempSnapshotZ80MemByteVector = _z80MemStuff->SetDiffParent(parentSnapshot->_z80MemStuff);
        std::unique_ptr<bytevector> tempSnapshotScreenByteVector = _screenBlob->SetDiffParent(parentSnapshot->_screenBlob);
        std::unique_ptr<bytevector> tempFloppyAByteVector = _floppyAImage->SetDiffParent(parentSnapshot->_floppyAImage);
        std::unique_ptr<bytevector> tempFloppyBByteVector = _floppyBImage->SetDiffParent(parentSnapshot->_floppyBImage);
        std::unique_ptr<bytevector> tempTapeByteVector = _tapeImage->SetDiffParent(parentSnapshot->_tapeImage);

        std::unique_ptr<CoreSnapshot> nextSnapshot = std::make_unique<CoreSnapshot>();
        nextSnapshot->_z80MemStuff = std::make_shared<Blob<SnapshotZ80Mem>>(tempSnapshotZ80MemByteVector);
        nextSnapshot->_screenBlob = std::make_shared<Blob<byte>>(tempSnapshotScreenByteVector);
        nextSnapshot->_floppyAImage = std::make_shared<Blob<byte>>(tempFloppyAByteVector);
        nextSnapshot->_floppyBImage = std::make_shared<Blob<byte>>(tempFloppyBByteVector);
        nextSnapshot->_tapeImage = std::make_shared<Blob<byte>>(tempTapeByteVector);

        return std::move(nextSnapshot);
    }

    // Serializable stuff that can be stored as a full image or a compressed diff.
    std::shared_ptr<Blob<SnapshotZ80Mem>> _z80MemStuff;
    std::shared_ptr<Blob<byte>> _screenBlob;
    std::shared_ptr<Blob<byte>> _floppyAImage;
    std::shared_ptr<Blob<byte>> _floppyBImage;
    std::shared_ptr<Blob<byte>> _tapeImage;

    // Stuff that is not yet serializable...
    Snapshot2nd64kAndRoms mem2nd64k;
    FDC _fdc;
    Tape _tape;
};

