#pragma once

#include <array>
#include <map>

#include "common.h"
#include "Blob.h"
#include "BlockPhase.h"
#include "Phase.h"

// Serialization for base types...
#define SERIALIZE_BASETYPE(T) \
__forceinline static uint64_t Size(const T& t) \
{ \
    return sizeof(t); \
} \
\
__forceinline static void Write(byte*& p, const T& t) \
{ \
    T* b = reinterpret_cast<T*>(p); \
    *b = t; \
    p += Size(t); \
} \
\
__forceinline static void Read(byte*& p, T& t) \
{ \
    T* b = reinterpret_cast<T*>(p); \
    t = *b; \
    p += Size(t); \
} \
\
template<typename... Args> \
__forceinline static uint64_t Size(const T& t, const Args&... args) \
{ \
    return Size(t) + Size(args...); \
} \
\
template<typename... Args> \
__forceinline static void Write(byte*& p, const T& t, const Args&... args) \
{ \
    Write(p, t); \
    Write(p, args...); \
} \
\
template<typename... Args> \
__forceinline static void Read(byte*& p, T& t, Args&... args) \
{ \
    Read(p, t); \
    Read(p, args...); \
}

#define SERIALIZE_MEMBERS(...) \
    __forceinline uint64_t SerializeSize() const { return Serialize::Size(__VA_ARGS__); } \
    __forceinline void SerializeWrite(byte*& p) const { Serialize::Write(p, __VA_ARGS__); } \
    __forceinline void SerializeRead(byte*& p) { Serialize::Read(p, __VA_ARGS__); }

#define SERIALIZE_MEMBERS_WITH_POSTREAD(...) \
    __forceinline uint64_t SerializeSize() const { return Serialize::Size(__VA_ARGS__); } \
    __forceinline void SerializeWrite(byte*& p) const { Serialize::Write(p, __VA_ARGS__); } \
    __forceinline void SerializeRead(byte*& p) { Serialize::Read(p, __VA_ARGS__); SerializePostRead(); }

class Serialize
{
public:
    Serialize()
    {
        
    }

    ~Serialize()
    {

    }

    SERIALIZE_BASETYPE(bool)
    SERIALIZE_BASETYPE(char)
    SERIALIZE_BASETYPE(offset)
    SERIALIZE_BASETYPE(int)
    SERIALIZE_BASETYPE(byte)
    SERIALIZE_BASETYPE(word)
    SERIALIZE_BASETYPE(dword)
    SERIALIZE_BASETYPE(qword)

    SERIALIZE_BASETYPE(Phase)
    SERIALIZE_BASETYPE(BlockPhase)

    // STL arrays
    template <int S>
    __forceinline static uint64_t Size(const std::array<byte, S>& a)
    {
        return a.size();
    }

    template <int S>
    __forceinline static void Write(byte*& p, const std::array<byte, S>& a)
    {
        memcpy(p, a.data(), a.size());
        p += Size(a);
    }

    template <int S>
    __forceinline static void Read(byte*& p, std::array<byte, S>& a)
    {
        memcpy(a.data(), p, S);
        p += Size(a);
    }

    template<class T, int S>
    __forceinline static uint64_t Size(const std::array<T, S>& x)
    {
        return S * Size(*((T*)nullptr));
    }

    template<class T, int S>
    __forceinline static void Write(byte*& p, const std::array<T, S>& x)
    {
        for (const T& t : x)
        {
            Write(p, t);
        }
    }

    template<class T, int S>
    __forceinline static void Read(byte*& p, std::array<T, S>& x)
    {
        for (size_t i = 0; i < S; i++)
        {
            Read(p, x[i]);
        }
    }

    template<class T, int S, typename... Args>
    __forceinline static uint64_t Size(const std::array<T, S>& x, const Args&... args)
    {
        return Size(x) + Size(args...);
    }

    template<class T, int S, typename... Args>
    __forceinline static void Write(byte*& p, const std::array<T, S>& x, const Args&... args)
    {
        Write(p, x);
        Write(p, args...);
    }

    template<class T, int S, typename... Args>
    __forceinline static void Read(byte*& p, std::array<T, S>& x, Args&... args)
    {
        Read(p, x);
        Read(p, args...);
    }

    // STL byte vectors
    __forceinline static uint64_t Size(const bytevector& v)
    {
        return Size(v.size()) + v.size();
    }

    __forceinline static void Write(byte*& p, const bytevector& v)
    {
        Serialize::Write(p, v.size());

        memcpy(p, v.data(), v.size());
        p += v.size();
    }

    __forceinline static void Read(byte*& p, bytevector& v)
    {
        size_t size = 0;
        Serialize::Read(p, size);
        v.resize(size);
        memcpy(v.data(), p, size);
        p += size;
    }

    template<typename... Args>
    __forceinline static uint64_t Size(const bytevector& x, const Args&... args)
    {
        return Size(x) + Size(args...);
    }

    template<typename... Args>
    __forceinline static void Write(byte*& p, const bytevector& x, const Args&... args)
    {
        Write(p, x);
        Write(p, args...);
    }

    template<typename... Args>
    __forceinline static void Read(byte*& p, bytevector& x, Args&... args)
    {
        Read(p, x);
        Read(p, args...);
    }

    // STL maps - base case
    template <typename K, typename V>
    __forceinline static uint64_t Size(const std::map<K, V>& m)
    {
        uint64_t mapSize = m.size();
        size_t size = Size(mapSize);
        size += (mapSize * (Size(*((K*)nullptr)) + Size(*((V*)nullptr))));

        return size;
    }

    template <typename K, typename V>
    __forceinline static void Write(byte*& p, const std::map<K, V>& m)
    {
        size_t count = m.size();
        Write(p, count);

        for (const std::pair<K, V>& kv : m)
        {
            Write(p, kv.first);
            Write(p, kv.second);
        }
    }

    template <typename K, typename V>
    __forceinline static void Read(byte*& p, std::map<K, V>& m)
    {
        size_t count = 0;
        Read(p, count);

        m.clear();
        for (size_t i = 0; i < count; i++)
        {
            K k;
            Read(p, k);

            V v;
            Read(p, v);

            m[k] = v;
        }
    }

    // STL maps - recursive case
    template <typename K, typename V, typename... Args>
    __forceinline static uint64_t Size(const std::map<K, V>& x, const Args&... args)
    {
        return Size(x) + Size(args...);
    }

    template <typename K, typename V, typename... Args>
    __forceinline static void Write(byte*& p, const std::map<K, V>& x, const Args&... args)
    {
        Write(p, x);
        Write(p, args...);
    }

    template <typename K, typename V, typename... Args>
    __forceinline static void Read(byte*& p, std::map<K, V>& x, Args&... args)
    {
        Read(p, x);
        Read(p, args...);
    }

    // C++ arrays - base case

    // Could this specialization for C++ byte arrays be moved to the macro instead (and thus be applicable for all base types)?
    template <int S>
    __forceinline static uint64_t Size(const byte(&a)[S])
    {
        return sizeof(a);
    }

    template <int S>
    __forceinline static void Write(byte*& p, const byte(&a)[S])
    {
        memcpy(p, a, S);
        p += Size(a);
    }

    template <int S>
    __forceinline static void Read(byte*& p, byte(&a)[S])
    {
        memcpy(a, p, S);
        p += Size(a);
    }

    template <typename X, int S>
    __forceinline static uint64_t Size(const X(&a)[S])
    {
        size_t s = 0;
        for (const X& x : a)
        {
            s += Size(x);
        }

        return s;
    }

    template <typename X, int S>
    __forceinline static void Write(byte*& p, const X(&a)[S])
    {
        for (const X& x : a)
        {
            Write(p, x);
        }
    }

    template <typename X, int S>
    __forceinline static void Read(byte*& p, X(&a)[S])
    {
        for (X& x : a)
        {
            Read(p, x);
        }
    }

    // C++ arrays - recursive case
    template<class T, int S, typename... Args>
    __forceinline static uint64_t Size(const T(&x)[S], const Args&... args)
    {
        return Size(x) + Size(args...);
    }

    template<class T, int S, typename... Args>
    __forceinline static void Write(byte*& p, const T(&x)[S], const Args&... args)
    {
        Write(p, x);
        Write(p, args...);
    }

    template<class T, int S, typename... Args>
    __forceinline static void Read(byte*& p, T(&x)[S], Args&... args)
    {
        Read(p, x);
        Read(p, args...);
    }

    // std::unique_ptr - base case
    template <typename X>
    __forceinline static uint64_t Size(const std::unique_ptr<X>& x)
    {
        size_t s = 0;

        s += Size(*((bool*)nullptr));
        if (x != nullptr)
        {
            s += Size(*x);
        }

        return s;
    }

    template <typename X>
    __forceinline static void Write(byte*& p, const std::unique_ptr<X>& x)
    {
        if (x != nullptr)
        {
            Write(p, true);
            Write(p, *x);
        }
        else
        {
            Write(p, false);
        }
    }

    template <typename X>
    __forceinline static void Read(byte*& p, std::unique_ptr<X>& x)
    {
        bool notNull = false;
        Read(p, notNull);
        if (notNull)
        {
            x = std::make_unique<X>();
            Read(p, *x);
        }
        else
        {
            x.reset();
        }
    }

    // std::unique_ptr - recursive case
    template<class X, typename... Args>
    __forceinline static uint64_t Size(const std::unique_ptr<X>& x, const Args&... args)
    {
        return Size(x) + Size(args...);
    }

    template<class X, typename... Args>
    __forceinline static void Write(byte*& p, const std::unique_ptr<X>& x, const Args&... args)
    {
        Write(p, x);
        Write(p, args...);
    }

    template<class X, typename... Args>
    __forceinline static void Read(byte*& p, std::unique_ptr<X>& x, Args&... args)
    {
        Read(p, x);
        Read(p, args...);
    }

    // Variadic template methods

    // Base case
    static constexpr uint64_t Size()
    {
        return 0;
    }

    static void Write(byte*& p)
    {
    }

    static void Read(byte*& p)
    {
    }

    // Recursive case
    template<class X, typename... Args>
    __forceinline static uint64_t Size(const X& x, const Args&... args)
    {
        return x.SerializeSize() + Size(args...);
    }

    template<class X, typename... Args>
    __forceinline static void Write(byte*& p, const X& x, const Args&... args)
    {
        x.SerializeWrite(p);
        Write(p, args...);
    }

    template<class X, typename... Args>
    __forceinline static void Read(byte*& p, X& x, Args&... args)
    {
        x.SerializeRead(p);
        Read(p, args...);
    }

public:
    template<class... Args>
    __forceinline static void Write(Blob& blob, const Args&... args)
    {
        size_t s = Size(args...);
        blob.SetCount(s);

        byte* p = blob.Data();
        Write(p, args...);
    }

    template<class... Args>
    __forceinline static void Read(Blob& blob, Args&... args)
    {
        byte* p = blob.Data();
        Read(p, args...);
    }

    template<class... Args>
    __forceinline static void Write(bytevector& bv, const Args&... args)
    {
        size_t s = Size(args...);
        bv.resize(s);

        byte* p = bv.data();
        Write(p, args...);
    }

    template<class... Args>
    __forceinline static void Read(bytevector& bv, Args&... args)
    {
        byte* p = bv.data();
        Read(p, args...);
    }
};
