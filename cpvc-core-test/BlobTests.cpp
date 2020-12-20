#include "gtest/gtest.h"
#include "../cpvc-core/Blob.h"

std::shared_ptr<Blob<byte>> NewBlob(const bytevector& bytes)
{
    std::shared_ptr<Blob<byte>> pBlob = std::make_shared<Blob<byte>>(bytes.size());

    memcpy_s(pBlob->Data(), pBlob->Size(), bytes.data(), bytes.size());

    return pBlob;
}

TEST(BlobTests, Data)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };

    // Act
    std::shared_ptr<Blob<byte>> pBlob = NewBlob(bytes);
    //Blob<bytevector> blob = Blob<bytevector>(bytes.data(), bytes.size());

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
    //std::shared_ptr<Blob<byte>> parentBlob = std::make_shared<Blob<byte>>(bytes.data(), bytes.size());

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
    //std::shared_ptr<Blob<bytevector>> parentBlob = std::make_shared<Blob<bytevector>>(parentBytes.data(), parentBytes.size());
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
    //std::shared_ptr<Blob<bytevector>> parentBlob = std::make_shared<Blob<bytevector>>(parentBytes.data(), parentBytes.size());
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
    //std::shared_ptr<Blob<bytevector>> parentBlob = std::make_shared<Blob<bytevector>>(parentBytes.data(), parentBytes.size());
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

//TEST(BlobTests, NullData)
//{
//    // Setup
//    bytevector bytes = { };
//
//    // Act
//    Blob<bytevector> blob = Blob<bytevector>(nullptr, 0);
//
//    // Verify
//    ASSERT_EQ(0, blob.Size());
//}

