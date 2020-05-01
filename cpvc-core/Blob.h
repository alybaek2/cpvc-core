#pragma once

#include "StreamReader.h"
#include "StreamWriter.h"

// Class encapsulating a fixed-length buffer allocated on the heap.
// Created to avoid allocating large byte arrays on the stack, which would cause warnings (C6262) about stack size being exceeded.
template<int S>
class Blob
{
public:
    Blob()
    {
        _bytes.resize(S);
    }

    Blob(const byte* pData) : Blob()
    {
        Copy(pData);
    }

    Blob(const Blob<S>& blob) : Blob(blob._bytes.data())
    {
    }

    ~Blob()
    {
    }

    void Fill(byte value)
    {
        memset(_bytes.data(), value, S);
    }

    operator byte* ()
    {
        return _bytes.data();
    };

    Blob<S>& operator=(const Blob<S>& blob)
    {
        Copy(blob._bytes.data());

        return *this;
    }

    byte& operator[](int i)
    {
        return _bytes[i];
    }

private:
    void Copy(const byte* pData)
    {
        memcpy(_bytes.data(), pData, S);
    }

    std::vector<byte> _bytes;

    friend StreamWriter& operator<<(StreamWriter& s, const Blob<S>& blob)
    {
        s.WriteArray(blob._bytes.data(), S);

        return s;
    }

    friend StreamReader& operator>>(StreamReader& s, Blob<S>& blob)
    {
        s.ReadArray(blob._bytes.data(), S);

        return s;
    }
};

