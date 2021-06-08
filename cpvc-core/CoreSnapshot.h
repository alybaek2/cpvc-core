#pragma once

#include "common.h"
#include "Blob.h"
#include "Serialize.h"

// Optimized core storage.
struct CoreSnapshot
{
public:
    CoreSnapshot()
    {
        _coreStuff = std::make_shared<Blob>();
    }

    CoreSnapshot(Core& core, std::shared_ptr<CoreSnapshot>& parentSnapshot)
    {
        Create(core, parentSnapshot);
    }

    void Create(Core& core, std::shared_ptr<CoreSnapshot>& parentSnapshot)
    {
        std::unique_ptr<bytevector> serializedCore;
        if (parentSnapshot == nullptr || parentSnapshot->_recycledCore == nullptr)
        {
            serializedCore = std::make_unique<bytevector>();
        }
        else
        {
            serializedCore = std::move(parentSnapshot->_recycledCore);
        }

        Serialize::Write(*serializedCore.get(), core);

        _coreStuff = std::make_shared<Blob>(serializedCore);

        if (parentSnapshot != nullptr)
        {
            _recycledCore = parentSnapshot->_coreStuff->SetDiffParent(_coreStuff);
        }
    }

    // Serializable stuff that can be stored as a full image or a compressed diff.
    std::shared_ptr<Blob> _coreStuff;

    std::unique_ptr<bytevector> _recycledCore;
};

