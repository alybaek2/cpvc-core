#include "gtest/gtest.h"
#include "../cpvc-core/Blob.h"

TEST(BlobTests, Data)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };

    // Act
    Blob blob = Blob(bytes.data(), bytes.size());

    // Verify
    ASSERT_EQ(bytes.size(), blob.Size());
    ASSERT_NE(nullptr, blob.Data());
    ASSERT_EQ(0, memcmp(bytes.data(), blob.Data(), blob.Size()));
}

TEST(BlobTests, DataWithParentSameBytes)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };
    std::shared_ptr<Blob> parentBlob = std::make_shared<Blob>(bytes.data(), bytes.size());

    // Act
    Blob blob = Blob(bytes.data(), bytes.size());
    blob.SetDiffParent(parentBlob);

    // Verify
    ASSERT_EQ(bytes.size(), blob.Size());
    ASSERT_NE(nullptr, blob.Data());
    ASSERT_EQ(0, memcmp(bytes.data(), blob.Data(), blob.Size()));
}

TEST(BlobTests, DataWithParentFewerBytes)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };
    bytevector parentBytes = { 0x99, 0x12, 0x06 };
    std::shared_ptr<Blob> parentBlob = std::make_shared<Blob>(parentBytes.data(), parentBytes.size());

    // Act
    Blob blob = Blob(bytes.data(), bytes.size());
    blob.SetDiffParent(parentBlob);

    // Verify
    ASSERT_EQ(bytes.size(), blob.Size());
    ASSERT_NE(nullptr, blob.Data());
    ASSERT_EQ(0, memcmp(bytes.data(), blob.Data(), blob.Size()));
}

TEST(BlobTests, DataWithParentMoreBytes)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };
    bytevector parentBytes = { 0x01, 0x42, 0x99, 0x12, 0x06 };
    std::shared_ptr<Blob> parentBlob = std::make_shared<Blob>(parentBytes.data(), parentBytes.size());

    // Act
    Blob blob = Blob(bytes.data(), bytes.size());
    blob.SetDiffParent(parentBlob);

    // Verify
    ASSERT_EQ(bytes.size(), blob.Size());
    ASSERT_NE(nullptr, blob.Data());
    ASSERT_EQ(0, memcmp(bytes.data(), blob.Data(), blob.Size()));
}

TEST(BlobTests, DataWithParentNoBytes)
{
    // Setup
    bytevector bytes = { 0x42, 0x99, 0x12, 0x06 };
    bytevector parentBytes = { };
    std::shared_ptr<Blob> parentBlob = std::make_shared<Blob>(parentBytes.data(), parentBytes.size());

    // Act
    Blob blob = Blob(bytes.data(), bytes.size());
    blob.SetDiffParent(parentBlob);

    // Verify
    ASSERT_EQ(bytes.size(), blob.Size());
    ASSERT_NE(nullptr, blob.Data());
    ASSERT_EQ(0, memcmp(bytes.data(), blob.Data(), blob.Size()));
}

TEST(BlobTests, NoData)
{
    // Setup
    bytevector bytes = { };

    // Act
    Blob blob = Blob(bytes.data(), bytes.size());

    // Verify
    ASSERT_EQ(0, blob.Size());
}

TEST(BlobTests, NullData)
{
    // Setup
    bytevector bytes = { };

    // Act
    Blob blob = Blob(nullptr, 0);

    // Verify
    ASSERT_EQ(0, blob.Size());
}

