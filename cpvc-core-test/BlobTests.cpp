#include "gtest/gtest.h"
#include "../cpvc-core/Blob.h"

std::shared_ptr<Blob> NewBlob(const bytevector& bytes)
{
    size_t size = bytes.size();
    std::shared_ptr<Blob> pBlob = std::make_shared<Blob>();
    pBlob->SetCount(size);

    memcpy_s(pBlob->Data(), pBlob->Size(), bytes.data(), size);

    return pBlob;
}

TEST(BlobTests, Data)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };

    // Act
    std::shared_ptr<Blob> pBlob = NewBlob(bytes);

    // Verify
    ASSERT_EQ(bytes.size(), pBlob->Size());
    ASSERT_NE(nullptr, pBlob->Data());
    ASSERT_EQ(0, memcmp(bytes.data(), pBlob->Data(), pBlob->Size()));
}

TEST(BlobTests, DataWithParentSameBytes)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };
    std::shared_ptr<Blob> parentBlob = NewBlob(bytes);

    // Act
    std::shared_ptr<Blob> pBlob = NewBlob(bytes);
    pBlob->SetDiffParent(parentBlob);

    // Verify
    ASSERT_EQ(bytes.size(), pBlob->Size());
    ASSERT_NE(nullptr, pBlob->Data());
    ASSERT_EQ(0, memcmp(bytes.data(), pBlob->Data(), pBlob->Size()));
}

TEST(BlobTests, DataWithParentFewerBytes)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };
    bytevector parentBytes = { 0x99, 0x12, 0x06 };
    std::shared_ptr<Blob> parentBlob = NewBlob(parentBytes);

    // Act
    std::shared_ptr<Blob> pBlob = NewBlob(bytes);
    pBlob->SetDiffParent(parentBlob);

    // Verify
    ASSERT_EQ(bytes.size(), pBlob->Size());
    ASSERT_NE(nullptr, pBlob->Data());
    ASSERT_EQ(0, memcmp(bytes.data(), pBlob->Data(), pBlob->Size()));
}

TEST(BlobTests, DataWithParentMoreBytes)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };
    bytevector parentBytes = { 0x01, 0x42, 0x99, 0x12, 0x06 };
    std::shared_ptr<Blob> parentBlob = NewBlob(parentBytes);

    // Act
    std::shared_ptr<Blob> pBlob = NewBlob(bytes);
    pBlob->SetDiffParent(parentBlob);

    // Verify
    ASSERT_EQ(bytes.size(), pBlob->Size());
    ASSERT_NE(nullptr, pBlob->Data());
    ASSERT_EQ(0, memcmp(bytes.data(), pBlob->Data(), pBlob->Size()));
}

TEST(BlobTests, DataWithParentNoBytes)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };
    bytevector parentBytes = { };
    std::shared_ptr<Blob> parentBlob = NewBlob(parentBytes);

    // Act
    std::shared_ptr<Blob> pBlob = NewBlob(bytes);
    pBlob->SetDiffParent(parentBlob);

    // Verify
    ASSERT_EQ(bytes.size(), pBlob->Size());
    ASSERT_NE(nullptr, pBlob->Data());
    ASSERT_EQ(0, memcmp(bytes.data(), pBlob->Data(), pBlob->Size()));
}

TEST(BlobTests, NoData)
{
    // Setup
    bytevector bytes = { };

    // Act
    std::shared_ptr<Blob> pBlob = NewBlob(bytes);

    // Verify
    ASSERT_EQ(0, pBlob->Size());
}

