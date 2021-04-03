#pragma once

#include "common.h"
#include "Encode.h"

/// <summary>
/// Class for storing a blob.
/// </summary>
/// <remarks>
/// By default, this class will store a single instance of T. The consturctor taking
/// <c>size</c> can be used to store multiple instances in one contiguous block.
/// </remarks>
class Blob
{
public:
    Blob()
    {
        std::unique_ptr<bytevector> pBytes(nullptr);
        Init(pBytes);
        SetCount(0);
    }

    Blob(std::unique_ptr<bytevector>& pBytes)
    {
        Init(pBytes);
    }

    ~Blob()
    {
    }

    /// <summary>
    /// Updates the internal representation of the blob to a "diff" with respect to another
    /// Blob object. Depending on how similar to the two Blobs are, this will result in less
    /// memory being used to store this Blob.
    /// </summary>
    /// <param name="parentBlob">Smart pointer to another Blob.</param>
    /// <returns>A std::unique_ptr<bytevector> of the storage that was previously being used to store the full, uncompressed object.</returns>
    std::unique_ptr<bytevector> SetDiffParent(std::shared_ptr<Blob>& parentBlob)
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

        byte* pDiffBytes = diffBytes.data();
        byte* pParentBytes = pParentData;
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

    void SetCount(size_t newCount)
    {
        if (_pBytes != nullptr)
        {
            _pBytes->resize(newCount);
        }
    }

private:
    void Init(std::unique_ptr<bytevector>& pBytes)
    {
        if (pBytes == nullptr)
        {
            _pBytes = std::make_unique<bytevector>(0);
        }
        else
        {
            _pBytes = std::move(pBytes);
        }
    }

    void Clear()
    {
        if (_pBytes != nullptr && _pBytes->size() > 0)
        {
            memset(_pBytes->data(), 0, _pBytes->size());
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
    std::shared_ptr<Blob> _parentBlob;
    std::unique_ptr<bytevector> _pEncodedDiffBytes;
};
