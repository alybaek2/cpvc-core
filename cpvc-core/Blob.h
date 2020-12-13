#pragma once

#include "common.h"

class Blob
{
public:
    Blob(size_t size = 0);
    Blob(byte* pBytes, size_t size = 0);
    ~Blob();

    byte* Data();
    size_t Size();
    void SetDiffParent(std::shared_ptr<Blob> parentBlob);
private:
    void PopulateBytes();

    bytevector _bytes;
    
    std::shared_ptr<Blob> _parentBlob;
    bytevector _encodedDiffBytes;
};

