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
    Blob(size_t count)
    {
        std::unique_ptr<bytevector> pBytes(nullptr);
        Init(count, pBytes);
    }

    Blob(size_t count, std::unique_ptr<bytevector>& pBytes)
    {
        Init(count, pBytes);
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
    /// <returns>A std::unique_ptr<bytevector> of the storage that was being used to store the full, uncompressed object.</returns>
    std::unique_ptr<bytevector> SetDiffParent(std::shared_ptr<Blob<T>>& parentBlob)
    {
        PopulateBytes();

        bytevector diffBytes = *_pBytes;
        size_t parentSize = parentBlob->Size();
        byte* pParentData = parentBlob->Data();

        size_t size = diffBytes.size();
        if (parentSize < size)
        {
            size = parentSize;
        }

        byte* pDiffBytes = &diffBytes[0];
        byte* pParentBytes = &pParentData[0];
        for (size_t i = 0; i < size; i++)
        {
            *pDiffBytes ^= *pParentBytes;

            pDiffBytes++;
            pParentBytes++;
        }

        _pEncodedDiffBytes = std::make_unique<bytevector>();
        RunLengthEncode(diffBytes.data(), _pBytes->size(), *_pEncodedDiffBytes);
        _parentBlob = parentBlob;
        _pBytes->clear();

        return std::move(_pBytes);
    }

    byte* Data()
    {
        PopulateBytes();

        return _pBytes->data();
    }

    size_t Size()
    {
        PopulateBytes();

        return _pBytes->size();
    }

private:
    void Init(size_t count, std::unique_ptr<bytevector>& pBytes)
    {
        size_t size = sizeof(T) * count;

        if (pBytes == nullptr)
        {
            _pBytes = std::make_unique<bytevector>(size);
        }
        else
        {
            _pBytes = std::move(pBytes);
            _pBytes->resize(size);
        }

        if (size > 0)
        {
            memset(_pBytes->data(), 0, size);
        }
    }

    void PopulateBytes()
    {
        if (_parentBlob != nullptr)
        {
            // We're a diff... switch back to a full image.
            bytevector diffBytes;
            RunLengthDecode(*_pEncodedDiffBytes, diffBytes);

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

            _pBytes = std::make_unique<bytevector>(diffBytes);
            _parentBlob = nullptr;
            _pEncodedDiffBytes->clear();
        }
    }

    // When not storing as a diff.
    std::unique_ptr<bytevector> _pBytes;

    // Members for when storing as a diff between this object and another.
    std::shared_ptr<Blob<T>> _parentBlob;
    std::unique_ptr<bytevector> _pEncodedDiffBytes;
};