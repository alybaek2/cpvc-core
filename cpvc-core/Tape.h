#pragma once

#include "common.h"
#include "stringify.h"

#include "Serialize.h"

#include "StreamReader.h"
#include "StreamWriter.h"

#include "BlockPhase.h"

// Macro for adjusting T-State values. Values in .CDT file are for
// a 3.5MHz clock, whereas we're using a 4MHz clock.
template<class T>
constexpr T AdjustTicks(T ticks) { return (8 * ticks) / 7; };

inline byte BlockByte(byte* pBlock, int offset)
{
    return pBlock[offset];
}

inline word BlockWord(byte* pBlock, int offset)
{
    return  ((word*)pBlock)[offset];
}

inline dword BlockTripleByte(byte* pBlock, int offset)
{
    return 0x00FFFFFF & ((dword*)pBlock)[offset];
}

struct SpeedBlockData
{
    SpeedBlockData()
    {
        _pilotPulseLength = 0;
        _sync1Length = 0;
        _sync2Length = 0;
        _pilotPulseCount = 0;
    }

    word _pilotPulseLength;
    word _sync1Length;
    word _sync2Length;
    word _pilotPulseCount;
};

struct DataBlock
{
    DataBlock()
    {
        _zeroLength = 0;
        _oneLength = 0;
        _usedBitsLastByte = 0;
        _pause = 0;
        _length = 0;
    }

    word _zeroLength;
    word _oneLength;
    byte _usedBitsLastByte;
    word _pause;
    dword _length;
};

StreamWriter& operator<<(StreamWriter& s, const BlockPhase& phase);
StreamReader& operator>>(StreamReader& s, BlockPhase& phase);

class Tape
{
public:
    Tape();
    ~Tape();

    bool _level;
    bool _motor;
    bool _playing;

    bool Load(const bytevector& buffer);

    void Rewind();
    void Eject();
    void Tick();

    bytevector _buffer;

    SERIALIZE_MEMBERS(
        _currentBlockIndex,
        _blockIndex,
        _phase,
        _pulsesRemaining,
        _dataIndex,
        _levelChanged,
        _dataByte,
        _remainingBits,
        _pulseIndex,
        _pause,
        _dataBlock._zeroLength,
        _dataBlock._oneLength,
        _dataBlock._usedBitsLastByte,
        _dataBlock._pause,
        _dataBlock._length,
        _speedBlock._pilotPulseLength,
        _speedBlock._sync1Length,
        _speedBlock._sync2Length,
        _speedBlock._pilotPulseCount,
        _playing,
        _level,
        _motor,
        _tickPos,
        _ticksToNextLevelChange,
        _buffer,
        _hasTape)

private:
    bool _hasTape;

    // Parsing members...
    int _currentBlockIndex;
    int _blockIndex;
    BlockPhase _phase;
    int _pulsesRemaining;
    dword _dataIndex;
    bool _levelChanged;
    byte _dataByte;
    int _remainingBits;
    int _pulseIndex;
    int _pause;

    DataBlock _dataBlock;
    SpeedBlockData _speedBlock;

    qword _tickPos;
    qword _ticksToNextLevelChange;

    qword TicksToNextLevelChange();

    dword BlockSize();

    qword DataPhase(byte* pData);
    void EndPhase();

    qword StepSpeedDataBlock(byte* pData);

    qword StepID10();
    qword StepID11();
    qword StepID12();
    qword StepID13();
    qword StepID14();
    qword StepID15();
    qword StepID20();

    friend StreamWriter& operator<<(StreamWriter& s, const Tape& tape);
    friend StreamReader& operator>>(StreamReader& s, Tape& tape);

    friend std::ostringstream& operator<<(std::ostringstream& s, const Tape& tape);
};
