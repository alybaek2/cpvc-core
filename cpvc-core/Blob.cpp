#include "Blob.h"
#include "Encode.h"

Blob::Blob(size_t size)
{
    _bytes.resize(size);

    if (size > 0)
    {
        memset(_bytes.data(), 0, size);
    }
}

Blob::Blob(byte* pBytes, size_t size)
{
    _bytes.resize(size);

    if (size > 0)
    {
        memcpy(_bytes.data(), pBytes, size);
    }
}

Blob::~Blob()
{
}

void Blob::SetDiffParent(std::shared_ptr<Blob> parentBlob)
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
}

byte* Blob::Data()
{
    PopulateBytes();

    return _bytes.data();
}

size_t Blob::Size()
{
    PopulateBytes();

    return _bytes.size();
}

void Blob::PopulateBytes()
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