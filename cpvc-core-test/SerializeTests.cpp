#include "gtest/gtest.h"
#include <vector>
#include "../cpvc-core/Serialize.h"

TEST(SerializeTests, WriteArray)
{
    // Setup
    std::array<byte, 4> a[2];
    memset(a[0].data(), 0x11, 4);
    memset(a[1].data(), 0x22, 4);

    // Act
    bytevector b;
    Serialize::Write(b, a);

    // Verify
    byte expected[] =
    {
        0x11, 0x11, 0x11, 0x11,
        0x22, 0x22, 0x22, 0x22
    };

    ASSERT_EQ(sizeof(expected), b.size());
    ASSERT_EQ(0, memcmp(expected, b.data(), b.size()));
}

TEST(SerializeTests, ReadArray)
{
    // Setup
    std::array<byte, 4> a[2];
    memset(a[0].data(), 0x11, 4);
    memset(a[1].data(), 0x22, 4);

    // Act
    Blob w;
    Serialize::Write(w, a);
    std::array<byte, 4> b[2];
    Blob r;
    Serialize::Read(w, b);

    // Verify
    ASSERT_EQ(0, memcmp(a, b, sizeof(a)));
}

TEST(SerializeTests, WriteMap)
{
    // Setup
    std::array<byte, 4> array1;
    memset(array1.data(), 0x11, 4);
    std::array<byte, 4> array2;
    memset(array2.data(), 0x22, 4);
    std::map<byte, std::array<byte, 4>> m;
    m[1] = array1;
    m[2] = array2;

    // Act
    bytevector b;
    Serialize::Write(b, m);

    // Verify
    byte expected[] =
    {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01,
        0x11, 0x11, 0x11, 0x11,
        0x02,
        0x22, 0x22, 0x22, 0x22
    };

    ASSERT_EQ(sizeof(expected), b.size());
    ASSERT_EQ(0, memcmp(expected, b.data(), b.size()));
}

template <typename T, int S>
void TestWriteAndRead(const T(&values)[S])
{
    for (const T& write : values)
    {
        // Setup
        bytevector serialized;

        // Make sure what we read back is initailly different from what we expect.
        T read = (write ^ ((T)-1));

        // Act
        Serialize::Write(serialized, write);
        Serialize::Read(serialized, read);

        // Verify
        ASSERT_EQ(write, read);
    }
}

TEST(SerializeTests, Bool)
{
    TestWriteAndRead({ false, true });
}

TEST(SerializeTests, Char)
{
    TestWriteAndRead<char>({ 'a', '\n', '9' });
}

TEST(SerializeTests, Byte)
{
    TestWriteAndRead<byte>({ 0x00, 0x0f, 0xf0, 0xff });
}

TEST(SerializeTests, Offset)
{
    TestWriteAndRead<offset>({ 0, -1, 1, 127, -128 });
}

TEST(SerializeTests, Word)
{
    TestWriteAndRead<word>({ 0x0000, 0x0f0f, 0xf0f0, 0xffff });
}

TEST(SerializeTests, Int)
{
    TestWriteAndRead<int>({ 0, -1, 1234567890 });
}

TEST(SerializeTests, Dword)
{
    TestWriteAndRead<dword>({ 0, 0x01234567, 0xffffffff });
}

TEST(SerializeTests, Qword)
{
    TestWriteAndRead<qword>({ 0, 0x0123456789abcdef, 0xffffffffffffffff });
}
