#pragma once

#include "common.h"
#include "StreamWriter.h"
#include <map>
#include <stdexcept>
#include <vector>

class StreamReader
{
public:
    StreamReader()
    {
        _bufferIndex = 0;
        _buffer.clear();
    }

    StreamReader(const StreamWriter& writer) : StreamReader()
    {
        _buffer.resize(writer.Size());
        writer.CopyTo(_buffer.data(), _buffer.size());
    }

    ~StreamReader()
    {
    }

    void Push(byte b)
    {
        _buffer.push_back(b);
    }

    void SetBuffer(byte* pData, size_t size)
    {
        _buffer.resize(size);
        memcpy(_buffer.data(), pData, size);
    }

    StreamReader& operator>>(bool& data)
    {
        return Read<bool>(data);
    }

    StreamReader& operator>>(byte& data)
    {
        return Read<byte>(data);
    }

    StreamReader& operator>>(char& data)
    {
        return Read<char>(data);
    }

    StreamReader& operator>>(signed char& data)
    {
        return Read<signed char>(data);
    }

    StreamReader& operator>>(word& data)
    {
        return Read<word>(data);
    }

    StreamReader& operator>>(dword& data)
    {
        return Read<dword>(data);
    }

    StreamReader& operator>>(int& data)
    {
        return Read<int>(data);
    }

    StreamReader& operator>>(qword& data)
    {
        return Read<qword>(data);
    }

    template<class T, int BUFSIZE>
    StreamReader& operator>>(T(&data)[BUFSIZE])
    {
        for (int i = 0; i < BUFSIZE; i++)
        {
            (*this) >> data[i];
        }

        return (*this);
    }

    template<class T, int S>
    StreamReader& operator>>(std::array<T, S>& arr)
    {
        for (size_t x = 0; x < S; x++)
        {
            (*this) >> arr[x];
        }

        return (*this);
    }

    template<typename T>
    StreamReader& operator>>(std::vector<T>& vector)
    {
        size_t size = 0;
        (*this) >> size;

        vector.resize(size);
        for (size_t x = 0; x < size; x++)
        {
            (*this) >> vector.at(x);
        }

        return (*this);
    }

    template<class K, class V>
    StreamReader& operator>>(std::map<K, V>& map)
    {
        size_t count = 0;
        (*this) >> count;

        map.clear();

        for (size_t i = 0; i < count; i++)
        {
            byte slot = 0;
            (*this) >> slot;
            (*this) >> map[slot];
        }

        return (*this);
    }

    template<typename T>
    void ReadArray(T* pArray, size_t size)
    {
        for (size_t x = 0; x < size; x++)
        {
            (*this) >> pArray[x];
        }
    }

    inline void ReadSimple(byte*& p, bool& b)
    {
        bool* pw = reinterpret_cast<bool*>(p);
        b = *pw;
        p += sizeof(b);
    }

    inline void ReadSimple(byte*& p, byte& b)
    {
        b = *p;
        p += sizeof(b);
    }

    inline void ReadSimple(byte*& p, word& w)
    {
        word* pw = reinterpret_cast<word*>(p);
        w = *pw;
        p += sizeof(w);
    }

    inline void ReadSimple(byte*& p, dword& d)
    {
        dword* pd = reinterpret_cast<dword*>(p);
        d = *pd;
        p += sizeof(d);
    }

    inline void ReadSimple(byte*& p, qword& w)
    {
        qword* pq = reinterpret_cast<qword*>(p);
        w = *pq;
        p += sizeof(w);
    }

    template<typename X, typename... Args>
    inline void ReadSimple(byte*& p, X& x, Args&... args)
    {
        ReadSimple(p, x);
        ReadSimple(p, args...);
    }

    template<typename X, typename... Args>
    inline void ReadSimple(X& x, Args&... args)
    {
        size_t s = ArgsSize(x, args...);

        byte* p = _buffer.data() + _bufferIndex;

        ReadSimple(p, x);
        ReadSimple(p, args...);

        _bufferIndex += s;
    }

private:
    size_t _bufferIndex;
    bytevector _buffer;

    template<typename T>
    StreamReader& Read(T& data)
    {
        int count = sizeof(data);
        if ((_bufferIndex + count) > _buffer.size())
        {
            throw std::out_of_range("No more data in the buffer");
        }

        memcpy((byte*)& data, _buffer.data() + _bufferIndex, count);
        _bufferIndex += count;

        return (*this);
    }
};
