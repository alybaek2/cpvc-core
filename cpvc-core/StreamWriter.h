#pragma once

#include "common.h"
#include <map>
#include <vector>

//#include "Serialize.h"

//template<typename X>
//inline size_t ArgsSize(const X& x)
//{
//    return sizeof(x);
//}
//
//template<typename X, typename... Args>
//inline size_t ArgsSize(const X& x, const Args&... args)
//{
//    return ArgsSize(x) + ArgsSize(args...);
//}
//
//inline void SerializeWrite(byte*& p, const bool& b)
//{
//    bool* pw = reinterpret_cast<bool*>(p);
//    *pw = b;
//    p += sizeof(b);
//}
//
//inline void SerializeWrite(byte*& p, const byte& b)
//{
//    *p = b;
//    p += sizeof(b);
//}
//
//inline void SerializeWrite(byte*& p, const word& w)
//{
//    word* pw = reinterpret_cast<word*>(p);
//    *pw = w;
//    p += sizeof(w);
//}
//
//inline void SerializeWrite(byte*& p, const dword& d)
//{
//    dword* pd = reinterpret_cast<dword*>(p);
//    *pd = d;
//    p += sizeof(d);
//}
//
//inline void SerializeWrite(byte*& p, const qword& w)
//{
//    qword* pq = reinterpret_cast<qword*>(p);
//    *pq = w;
//    p += sizeof(w);
//}
//
//template <int S>
//inline void SerializeWrite(byte*& p, const byte(&a)[S])
//{
//    memcpy(p, a, S);
//    p += S;
//}
//
//template<typename X, typename... Args>
//inline void SerializeWrite(byte*& p, const X& x, const Args&... args)
//{
//    SerializeWrite(p, x);
//    SerializeWrite(p, args...);
//}




class StreamWriter
{
public:
    StreamWriter()
    {
        _buffer.clear();
    };

    ~StreamWriter()
    {
    };

    size_t Size() const
    {
        return _buffer.size();
    }

    void Reserve(size_t capacity)
    {
        _buffer.reserve(capacity);
    }

    size_t CopyTo(byte* pBuffer, size_t bufferSize) const
    {
        size_t bytesToCopy = Size();
        if (bytesToCopy > bufferSize)
        {
            bytesToCopy = bufferSize;
        }

        memcpy(pBuffer, _buffer.data(), bytesToCopy);

        return bytesToCopy;
    }

    size_t CopyTo(bytevector& bv) const
    {
        bv.resize(Size());

        return CopyTo(bv.data(), bv.size());
    }

    StreamWriter& operator<<(byte data)
    {
        return Write<byte>(data);
    }

    StreamWriter& operator<<(char data)
    {
        return Write<char>(data);
    }

    StreamWriter& operator<<(signed char data)
    {
        return Write<signed char>(data);
    }

    StreamWriter& operator<<(bool data)
    {
        return Write<bool>(data);
    }

    StreamWriter& operator<<(word data)
    {
        return Write<word>(data);
    }

    StreamWriter& operator<<(dword data)
    {
        return Write<dword>(data);
    }

    StreamWriter& operator<<(int data)
    {
        return Write<int>(data);
    }

    StreamWriter& operator<<(qword data)
    {
        return Write<qword>(data);
    }

    template<class T, int BUFSIZE>
    StreamWriter& operator<<(T(&data)[BUFSIZE])
    {
        for (int i = 0; i < BUFSIZE; i++)
        {
            (*this) << data[i];
        }

        return (*this);
    }

    template<class T, int S>
    StreamWriter& operator<<(const std::array<T, S>& arr)
    {
        for (size_t x = 0; x < S; x++)
        {
            (*this) << arr.at(x);
        }

        return (*this);
    }

    template<class T>
    StreamWriter& operator<<(const std::vector<T>& vector)
    {
        size_t size = vector.size();
        (*this) << size;

        for (size_t x = 0; x < size; x++)
        {
            (*this) << vector.at(x);
        }

        return (*this);
    }

    template<class K, class V>
    StreamWriter& operator<<(const std::map<K, V>& map)
    {
        size_t count = map.size();
        (*this) << count;

        for (std::pair<K, V> kv : map)
        {
            (*this) << kv.first;
            (*this) << kv.second;
        }

        return (*this);
    }

    template<typename T>
    void WriteArray(T* pArray, size_t size)
    {
        for (size_t x = 0; x < size; x++)
        {
            (*this) << pArray[x];
        }
    }

    template<typename X>
    inline void WriteSimpleEx(const X& x)
    {
        size_t s = ArgsSize(x);

        size_t oldSize = _buffer.size();
        _buffer.resize(oldSize + s);
        byte* p = _buffer.data() + oldSize;

        SerializeWrite(p, x);
        //SerializeWrite(p, args...);
    }

    //template<typename X, typename... Args>
    //inline void WriteSimpleEx(const X& x, const Args&... args)
    //{
    //    size_t s = ArgsSize(args...);

    //    size_t oldSize = _buffer.size();
    //    _buffer.resize(oldSize + s);
    //    byte* p = _buffer.data() + oldSize;

    //    SerializeWrite(p, x);
    //    SerializeWrite(p, args...);
    //}

    //template<typename... Args>
    //inline void WriteSimpleEx(const Args&... args)
    //{
    //    size_t s = ArgsSize(args...);

    //    size_t oldSize = _buffer.size();
    //    _buffer.resize(oldSize + s);
    //    byte* p = _buffer.data() + oldSize;

    //    //SerializeWrite(p, x);
    //    SerializeWrite(p, args...);
    //}

private:
    bytevector _buffer;

    template<typename T>
    StreamWriter& Write(const T& data)
    {
        size_t size = _buffer.size();
        _buffer.resize(size + sizeof(data));
        memcpy_s(_buffer.data() + size, sizeof(data), (byte*)&data, sizeof(data));

        return (*this);
    }

    void Write(void* pData, size_t offset, size_t size)
    {
        size_t oldSize = _buffer.size();
        size_t newSize = oldSize + size;
        _buffer.resize(newSize);
        memcpy_s(_buffer.data() + oldSize, size, ((byte*)pData) + offset, size);
    }
};

