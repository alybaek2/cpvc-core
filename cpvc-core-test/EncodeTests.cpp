#include "gtest/gtest.h"
#include "../cpvc-core/Core.h"
#include "helpers.h"
#include "../cpvc-core/Encode.h"

TEST(EncodeTests, EncodeAndDecode)
{
    // Setup
    bytevector bytes = {
        0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
        0x11, 0x11, 0x11, 0x11, 0x22, 0x22, 0x22, 0x22, 0x22, 0x98, 0x99
    };

    bytes.reserve(0x1000000);
    for (int i = 0; i < 0x1000000; i++)
    {
        bytes.push_back(0xff);
    }

    bytes.push_back(0xfe);

    bytevector encoded;
    bytevector decoded;

    // Act
    RunLengthEncode(bytes.data(), bytes.size(), encoded);
    RunLengthDecode(encoded, decoded);

    // Verify
    ASSERT_EQ(bytes.size(), decoded.size());
    ASSERT_EQ(0, memcmp(bytes.data(), decoded.data(), bytes.size()));
};
