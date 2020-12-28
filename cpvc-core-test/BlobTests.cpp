#include "gtest/gtest.h"
#include "../cpvc-core/Blob.h"

std::shared_ptr<Blob<byte>> NewBlob(const bytevector& bytes)
{
    size_t size = bytes.size();
    std::shared_ptr<Blob<byte>> pBlob = std::make_shared<Blob<byte>>(size);

    memcpy_s(pBlob->Data(), pBlob->Size(), bytes.data(), size);

    return pBlob;
}

TEST(BlobTests, Data)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };

    // Act
    std::shared_ptr<Blob<byte>> pBlob = NewBlob(bytes);

    // Verify
    ASSERT_EQ(bytes.size(), pBlob->Size());
    ASSERT_NE(nullptr, pBlob->Data());
    ASSERT_EQ(0, memcmp(bytes.data(), pBlob->Data(), pBlob->Size()));
}

TEST(BlobTests, DataWithParentSameBytes)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };
    std::shared_ptr<Blob<byte>> parentBlob = NewBlob(bytes);

    // Act
    std::shared_ptr<Blob<byte>> pBlob = NewBlob(bytes);
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
    std::shared_ptr<Blob<byte>> parentBlob = NewBlob(parentBytes);

    // Act
    std::shared_ptr<Blob<byte>> pBlob = NewBlob(bytes);
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
    std::shared_ptr<Blob<byte>> parentBlob = NewBlob(parentBytes);

    // Act
    std::shared_ptr<Blob<byte>> pBlob = NewBlob(bytes);
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
    std::shared_ptr<Blob<byte>> parentBlob = NewBlob(parentBytes);

    // Act
    std::shared_ptr<Blob<byte>> pBlob = NewBlob(bytes);
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
    std::shared_ptr<Blob<byte>> pBlob = NewBlob(bytes);

    // Verify
    ASSERT_EQ(0, pBlob->Size());
}

TEST(BlobTests, MultipleObjects)
{
    // Act
    std::shared_ptr<Blob<__int64>> pBlob = std::make_shared<Blob<__int64>>(2);

    // Verify
    ASSERT_EQ(2 * sizeof(__int64), pBlob->Size());
}

TEST(BlobTests, MultipleObjectsWithEmptyStorage)
{
    // Setup
    std::unique_ptr<bytevector> pBytes = std::make_unique<bytevector>();
    pBytes->resize(1);

    // Act
    std::shared_ptr<Blob<__int64>> pBlob = std::make_shared<Blob<__int64>>(2, pBytes);

    // Verify
    ASSERT_EQ(2 * sizeof(__int64), pBlob->Size());
}

TEST(BlobTests, MultipleObjectsWithStorage)
{
    // Setup
    std::unique_ptr<bytevector> pBytes = std::make_unique<bytevector>();
    pBytes->resize(8);

    // Act
    std::shared_ptr<Blob<__int64>> pBlob = std::make_shared<Blob<__int64>>(2, pBytes);

    // Verify
    ASSERT_EQ(2 * sizeof(__int64), pBlob->Size());
}

TEST(BlobTests, MultipleObjectsWithExcessiveStorage)
{
    // Setup
    std::unique_ptr<bytevector> pBytes = std::make_unique<bytevector>();
    pBytes->resize(9);

    // Act
    std::shared_ptr<Blob<__int64>> pBlob = std::make_shared<Blob<__int64>>(2, pBytes);

    // Verify
    ASSERT_EQ(2 * sizeof(__int64), pBlob->Size());
}

TEST(BlobTests, ReferenceOperator)
{
    // Setup
    std::shared_ptr<Blob<__int64>> pBlob = std::make_shared<Blob<__int64>>(1);

    // Act
    __int64& i = *pBlob;

    // Verify
    ASSERT_EQ((byte*)&i, pBlob->Data());
}

