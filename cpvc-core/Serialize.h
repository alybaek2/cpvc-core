#pragma once

#include "common.h"

#include "Blob.h"

#include "Keyboard.h"
#include "Memory.h"
#include "PPI.h"
#include "PSG.h"
#include "GateArray.h"
#include "CRTC.h"
#include "FDC.h"
#include "Tape.h"
#include "Core.h"

// Serialization for base types...
#define SERIALIZE_BASETYPE(T) \
__forceinline uint64_t SerializeSize(const T& t) \
{ \
    return sizeof(t); \
} \
\
__forceinline void SerializeWrite(byte*& p, const T& t) \
{ \
    T* b = reinterpret_cast<T*>(p); \
    *b = t; \
    p += SerializeSize(t); \
} \
\
__forceinline void SerializeRead(byte*& p, T& t) \
{ \
    T* b = reinterpret_cast<T*>(p); \
    t = *b; \
    p += SerializeSize(t); \
}

SERIALIZE_BASETYPE(bool)
SERIALIZE_BASETYPE(char)
SERIALIZE_BASETYPE(int8_t)
SERIALIZE_BASETYPE(int)
SERIALIZE_BASETYPE(byte)
SERIALIZE_BASETYPE(word)
SERIALIZE_BASETYPE(dword)
SERIALIZE_BASETYPE(qword)
SERIALIZE_BASETYPE(Tape::BlockPhase)
SERIALIZE_BASETYPE(FDC::Phase)

template <int S>
__forceinline uint64_t SerializeSize(const byte(&a)[S])
{
    return sizeof(a);
}

template <int S>
__forceinline void SerializeWrite(byte*& p, const byte(&a)[S])
{
    memcpy(p, a, S);
    p += SerializeSize(a);
}

template <int S>
__forceinline void SerializeRead(byte*& p, byte(&a)[S])
{
    memcpy(a, p, S);
    p += SerializeSize(a);
}

template <int S>
__forceinline uint64_t SerializeSize(const std::array<byte, S>& a)
{
    return a.size();
}

template <int S>
__forceinline void SerializeWrite(byte*& p, const std::array<byte, S>& a)
{
    memcpy(p, a.data(), a.size());
    p += a.size();
}

template <int S>
__forceinline void SerializeRead(byte*& p, std::array<byte, S>& a)
{
    memcpy(a.data(), p, S);
    p += SerializeSize(a);
}

__forceinline uint64_t SerializeSize(const bytevector& v)
{
    return SerializeSize(v.size()) + v.size();
}

__forceinline void SerializeWrite(byte*& p, const bytevector& v)
{
    SerializeWrite(p, v.size());

    memcpy(p, v.data(), v.size());
    p += v.size();
}

__forceinline void SerializeRead(byte*& p, bytevector& v)
{
    size_t size = 0;
    SerializeRead(p, size);
    v.resize(size);
    memcpy(v.data(), p, size);
    p += size;
}

template <typename X, int S>
__forceinline uint64_t SerializeSize(const X(&a)[S])
{
    size_t s = 0;
    for (const X& x : a)
    {
        s += SerializeSize(x);
    }

    return s;
}

template <typename X, int S>
__forceinline void SerializeWrite(byte*& p, const X(&a)[S])
{
    for (const X& x : a)
    {
        SerializeWrite(p, x);
    }
}

template <typename X, int S>
__forceinline void SerializeRead(byte*& p, X(&a)[S])
{
    for (X& x : a)
    {
        SerializeRead(p, x);
    }
}

template <typename K, typename V>
__forceinline uint64_t SerializeSize(const std::map<K, V>& m)
{
    uint64_t mapSize = m.size();
    size_t size = SerializeSize(mapSize);
    size += (mapSize * (SerializeSize(*((K*)nullptr)) + SerializeSize(*((V*)nullptr))));

    return size;
}

template <typename K, typename V>
__forceinline void SerializeWrite(byte*& p, const std::map<K, V>& m)
{
    size_t count = m.size();
    SerializeWrite(p, count);

    for (const std::pair<K, V>& kv : m)
    {
        SerializeWrite(p, kv.first);
        SerializeWrite(p, kv.second);
    }
}

template <typename K, typename V>
__forceinline void SerializeRead(byte*& p, std::map<K, V>& m)
{
    size_t count = 0;
    SerializeRead(p, count);

    m.clear();
    for (size_t i = 0; i < count; i++)
    {
        K k;
        SerializeRead(p, k);

        V v;
        SerializeRead(p, v);

        m[k] = v;
    }
}

template<typename X, typename... Args>
__forceinline uint64_t SerializeSize(const X& x, const Args&... args)
{
    return SerializeSize(x) + SerializeSize(args...);
}

template<typename X, typename... Args>
__forceinline void SerializeWrite(byte*& p, const X& x, const Args&... args)
{
    SerializeWrite(p, x);
    SerializeWrite(p, args...);
}

template<typename X, typename... Args>
__forceinline void SerializeRead(byte*& p, X& x, Args&... args)
{
    SerializeRead(p, x);
    SerializeRead(p, args...);
}

#define SERIALIZE(T, ...) \
    __forceinline uint64_t SerializeSize(const T& t) {  return SerializeSize(__VA_ARGS__); } \
    __forceinline void SerializeWrite(byte*& p, const T& t) { SerializeWrite(p, __VA_ARGS__); } \
    __forceinline void SerializeRead(byte*& p, T& t) { SerializeRead(p, __VA_ARGS__); }

SERIALIZE(Keyboard,
    t._matrix,
    t._matrixClash,
    t._selectedLine)

SERIALIZE(Memory,
    t._banks,
    t._ramConfig,
    t._lowerRomEnabled,
    t._upperRomEnabled,
    t._selectedUpperRom,
    t._lowerRom,
    t._roms)

SERIALIZE(CRTC,
    t._x,
    t._y,
    t._hCount,
    t._vCount,
    t._raster,
    t._inHSync,
    t._hSyncCount,
    t._inVSync,
    t._vSyncCount,
    t._inVTotalAdjust,
    t._vTotalAdjustCount,
    t._scanLineCount,
    t._vSyncDelay,
    t._memoryAddress,
    t._register,
    t._selectedRegister)

SERIALIZE(PSG,
    t._bdir,
    t._bc1,
    t._selectedRegister,
    t._register,
    t._toneTicks,
    t._state,
    t._noiseTicks,
    t._noiseAmplitude,
    t._noiseRandom,
    t._envelopeTickCounter,
    t._envelopeStepCount,
    t._envelopePeriodCount,
    t._envelopeState,
    t._noiseTickCounter,
    t._envelopeStepState)

SERIALIZE(PPI,
    t._printerReady,
    t._exp,
    t._refreshRate,
    t._manufacturer,
    t._tapeWriteData,
    t._portA,
    t._portB,
    t._portC,
    t._control)

SERIALIZE(GateArray,
    t._selectedPen,
    t._pen,
    t._border,
    t._mode)

__forceinline uint64_t SerializeSize(const FDD& fdd)
{
    size_t size = SerializeSize(fdd._currentSector, fdd._currentTrack, fdd._hasDisk);

    if (fdd._hasDisk)
    {
        size += SerializeSize(fdd._diskImage);
    }

    return size;
}

__forceinline void SerializeWrite(byte*& p, const FDD& fdd)
{
    SerializeWrite(p, fdd._currentSector, fdd._currentTrack, fdd._hasDisk);

    if (fdd._hasDisk)
    {
        SerializeWrite(p, fdd._diskImage);
    }
}

__forceinline void SerializeRead(byte*& p, FDD& fdd)
{
    SerializeRead(p, fdd._currentSector, fdd._currentTrack, fdd._hasDisk);

    if (fdd._hasDisk)
    {
        SerializeRead(p, fdd._diskImage);
    }
}

SERIALIZE(Tape,
    t._currentBlockIndex,
    t._blockIndex,
    t._phase,
    t._pulsesRemaining,
    t._dataIndex,
    t._levelChanged,
    t._dataByte,
    t._remainingBits,
    t._pulseIndex,
    t._pause,
    t._dataBlock._zeroLength,
    t._dataBlock._oneLength,
    t._dataBlock._usedBitsLastByte,
    t._dataBlock._pause,
    t._dataBlock._length,
    t._speedBlock._pilotPulseLength,
    t._speedBlock._sync1Length,
    t._speedBlock._sync2Length,
    t._speedBlock._pilotPulseCount,
    t._playing,
    t._level,
    t._motor,
    t._tickPos,
    t._ticksToNextLevelChange,
    t._buffer,
    t._hasTape)

SERIALIZE(FDC,
    t._drives,
    t._readTimeout,
    t._mainStatus,
    t._data,
    t._dataDirection,
    t._motor,
    t._currentDrive,
    t._currentHead,
    t._status,
    t._seekCompleted,
    t._statusChanged,
    t._phase,
    t._commandBytes,
    t._commandByteCount,
    t._execBytes,
    t._execByteCount,
    t._execIndex,
    t._resultBytes,
    t._resultByteCount,
    t._resultIndex,
    t._stepReadTime,
    t._headLoadTime,
    t._headUnloadTime,
    t._nonDmaMode,
    t._readBuffer,
    t._readBufferIndex)

SERIALIZE(Core,
    t.AF,
    t.BC,
    t.DE,
    t.HL,
    t.IR,
    t.AF_,
    t.BC_,
    t.DE_,
    t.HL_,
    t.IX,
    t.IY,
    t.PC,
    t.SP,
    t._iff1,
    t._iff2,
    t._interruptRequested,
    t._interruptMode,
    t._eiDelay,
    t._halted,
    t._keyboard,
    t._crtc,
    t._psg,
    t._ppi,
    t._gateArray,
    t._ticks,
    t._frequency,
    t._audioTickTotal,
    t._audioTicksToNextSample,
    t._audioSampleCount,
    t._scrHeight,
    t._scrPitch,
    t._memory,
    t._fdc,
    t._tape,
    t._screen)

class Serialize
{
public:
    Serialize()
    {
        
    }

    ~Serialize()
    {

    }

    template<class... Args>
    __forceinline static void Write(Blob& blob, const Args&... args)
    {
        size_t s = SerializeSize(args...);
        blob.SetCount(s);

        byte* p = blob.Data();
        SerializeWrite(p, args...);
    }

    template<class... Args>
    __forceinline static void Read(Blob& blob, Args&... args)
    {
        byte* p = blob.Data();
        SerializeRead(p, args...);
    }

    template<class... Args>
    __forceinline static void Write(bytevector& bv, const Args&... args)
    {
        size_t s = SerializeSize(args...);
        bv.resize(s);

        byte* p = bv.data();
        SerializeWrite(p, args...);
    }

    template<class... Args>
    __forceinline static void Read(bytevector& bv, Args&... args)
    {
        byte* p = bv.data();
        SerializeRead(p, args...);
    }
    };
