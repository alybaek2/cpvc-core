#pragma once

#include "common.h"
#include "Encode.h"

/// <summary>
/// Template class that can be used to serialize simple types; specifically, types that
/// are stored entirely in a contiguous block of memory. Basically, the type must contain
/// no pointers or references.
/// </summary>
/// <remarks>
/// By default, this class will store a single instance of T. The consturctor taking
/// <c>size</c> can be used to store multiple instances in one contiguous block.
/// </remarks>
/// <typeparam name="T">Type the class will serialize</typeparam>
template <class T>
class Blob
{
public:
    /// <summary>
    /// Constructor that stores the specified number of instances of <c>T</c>.
    /// </summary>
    /// <param name="size">The number of instances of <c>T</c> to store.</param>
    Blob(size_t count = 1)
    {
        size_t size = sizeof(T) * count;
        _bytes.resize(size);

        if (count > 0)
        {
            memset(_bytes.data(), 0, size);
        }
    }

    ~Blob()
    {
    }

    /// <summary>
    /// Casting operator that retrieves the first instance of <c>T</c> as a reference.
    /// </summary>
    operator T& ()
    {
        return *((T*)Data());
    }

    /// <summary>
    /// If another object of the same type is suspected to have a similar serialization to the
    /// current set of objects, then this method will update its internal storage to reference
    /// the other object and (hopefully) save space.
    /// </summary>
    /// <param name="parentBlob">Smart pointer to the other object with a similar serialization.</param>
    void SetDiffParent(std::shared_ptr<Blob<T>>& parentBlob)
    {
        PopulateBytes();

        bytevector diffBytes = _bytes;
        size_t parentSize = parentBlob->Size();
        byte* pParentData = parentBlob->Data();

        size_t size = diffBytes.size();
        if (parentSize < size)
        {
            size = parentSize;
        }

        for (size_t i = 0; i < size; i++)
        {
            diffBytes[i] ^= pParentData[i];
        }

        RunLengthEncode(diffBytes.data(), _bytes.size(), _encodedDiffBytes);
        _parentBlob = parentBlob;
        _bytes.clear();
        bytevector().swap(_bytes);
    }

    byte* Data()
    {
        PopulateBytes();

        return _bytes.data();
    }

    size_t Size()
    {
        PopulateBytes();

        return _bytes.size();
    }

private:
    void PopulateBytes()
    {
        if (_parentBlob != nullptr)
        {
            // We're a diff... switch back to a full image.
            bytevector diffBytes;
            RunLengthDecode(_encodedDiffBytes, diffBytes);

            size_t parentSize = _parentBlob->Size();
            byte* pParentData = _parentBlob->Data();

            size_t size = diffBytes.size();
            if (parentSize < size)
            {
                size = parentSize;
            }

            for (size_t i = 0; i < size; i++)
            {
                diffBytes[i] ^= pParentData[i];
            }

            _bytes = diffBytes;
            _parentBlob = nullptr;
            _encodedDiffBytes.clear();
        }
    }

    // When not storing as a diff.
    bytevector _bytes;

    // Members for when storing as a diff between this object and another.
    std::shared_ptr<Blob<T>> _parentBlob;
    bytevector _encodedDiffBytes;
};