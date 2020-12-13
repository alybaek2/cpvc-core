#include "gtest/gtest.h"
#include "../cpvc-core/Core.h"
#include "helpers.h"

bytevector CoreState(Core* pCore)
{
    StreamWriter sw;
    sw << *pCore;
    bytevector state;
    sw.CopyTo(state);

    return state;
}

void SetCoreNonDefaultValues(Core* pCore)
{
    pCore->AF = 0x0123;
    pCore->BC = 0x4567;
    pCore->DE = 0x89ab;
    pCore->HL = 0xcdef;
    pCore->IR = 0x0246;

    pCore->AF_ = 0x3210;
    pCore->BC_ = 0x7654;
    pCore->DE_ = 0x89ab;
    pCore->HL_ = 0xfedc;

    pCore->IX = 0xaabb;
    pCore->IY = 0xccdd;

    pCore->PC = 0x1122;
    pCore->SP = 0x3344;

    pCore->_iff1 = true;
    pCore->_iff2 = true;
    pCore->_interruptRequested = true;
    pCore->_interruptMode = 0xcd;
    pCore->_eiDelay = 0xef;
    pCore->_halted = true;

    for (dword addr = 0; addr < 0x10000; addr++)
    {
        pCore->WriteRAM((word) addr, Low(Low(addr)));
    }
}

TEST(CoreTests, SetLowerRom)
{
    // Setup
    Mem16k lowerRom;
    lowerRom.fill(0xff);

    std::unique_ptr<Core> pCore = std::make_unique<Core>();
    pCore->SetLowerRom(lowerRom);

    // Act
    pCore->EnableLowerROM(true);
    byte enabledByte = pCore->ReadRAM(0x0000);

    pCore->EnableLowerROM(false);
    byte disabledByte = pCore->ReadRAM(0x0000);

    // Verify
    ASSERT_EQ(enabledByte, 0xff);
    ASSERT_EQ(disabledByte, 0x00);
};

TEST(CoreTests, SetUpperRom)
{
    // Setup
    Mem16k upperRom;
    upperRom.fill(0xff);

    std::unique_ptr<Core> pCore = std::make_unique<Core>();
    pCore->SetUpperRom(0, upperRom); // Default selected upper rom slot is 0.

    // Act
    pCore->EnableUpperROM(true);
    byte enabledByte = pCore->ReadRAM(0xc000);

    pCore->EnableUpperROM(false);
    byte disabledByte = pCore->ReadRAM(0xc000);

    // Verify
    ASSERT_EQ(enabledByte, 0xff);
    ASSERT_EQ(disabledByte, 0x00);
};

// Ensures that when writing to the screen buffer, the core stops once it hits the
// right-hand edge of the screen.
TEST(CoreTests, SetSmallWidthScreen)
{
    // Setup
    std::unique_ptr<Core> pCore = std::make_unique<Core>();

    // Create a screen buffer with a normal height, but small width.
    constexpr word widthChars = 10;
    constexpr word widthPixels = widthChars * 16;  // 16 pixels per CRTC char.
    constexpr word height = 300;
    constexpr int bufsize = widthPixels * height;

    byte* pScreen = new byte[bufsize];
    memset(pScreen, 1, bufsize);
    pCore->SetScreen(pScreen, widthPixels, height, widthPixels);

    // Act - since one CRTC "char" is written every 4 ticks, run the core for the full width
    //       of the screen plus one, and ensure this one extra char does not get written to
    //       the screen buffer. Note that we need to first run the core for 16 scanlines to
    //       compensate for overscan, hence the 16 * 0x40 below (0x40 chars is the default
    //       "Horizontal Total" for the CPC).
    pCore->RunUntil(((16 * 0x40) + widthChars + 1) * 4, 0, nullptr);

    // Verify
    for (word i = 0; i < bufsize; i++)
    {
        // The core should have written a single line of zero pixels in the screen buffer, and
        // the original ones in the second line should remain.
        ASSERT_EQ(pScreen[i], (i < widthPixels) ? 0 : 1);
    }

    delete[] pScreen;
}

// Ensures that when writing to the screen buffer, the core stops once it hits the
// bottom edge of the screen.
TEST(CoreTests, SetSmallHeightScreen)
{
    // Setup
    std::unique_ptr<Core> pCore = std::make_unique<Core>();

    // Create a screen buffer with a normal width, but small height.
    constexpr word widthChars = 40;
    constexpr word widthPixels = widthChars * 16;  // 16 pixels per CRTC char.
    constexpr word height = 2;
    constexpr int bufsize = widthPixels * height;

    // Allocate enough space for two lines, but tell the core the buffer height is one less than that.
    byte* pScreen = new byte[bufsize];
    memset(pScreen, 1, bufsize);
    pCore->SetScreen(pScreen, widthPixels, height - 1, widthPixels);

    // Act - run the core for two lines plus the number of overscan lines. Adding overscan lines is necessary
    //       to ensure we actually write into the screen buffer. The default total width is 0x40 chars, so
    //       double this for two lines. Note that one CRTC "char" is written every 4 ticks.
    pCore->RunUntil((0x40 * (2 + 16)) * 4, 0, nullptr);

    // Verify - ensure that only a single line was written.
    for (word i = 0; i < bufsize; i++)
    {
        // The core should have written a single line of zero pixels in the screen buffer, and
        // the original ones in the second line should remain.
        ASSERT_EQ(pScreen[i], (i < widthPixels) ? 0 : 1);
    }

    delete[] pScreen;
}

// Tests that a core can be serialized and deserialized back to the same state. Note this test could probably be improved to
// ensure the core is initially in a state where registers, memory, etc. aren't all zeros and thus more likely to catch errors
// when serialized then deserialized.
TEST(CoreTests, SerializeDeserialize)
{
    // Setup
    std::unique_ptr<Core> pCore = std::make_unique<Core>();
    StreamWriter writer;
    writer << (*pCore);
    bytevector state1;
    writer.CopyTo(state1);
    StreamReader reader(writer);

    // Act
    std::unique_ptr<Core> pCore2 = std::make_unique<Core>();
    reader >> (*pCore2);
    StreamWriter writer2;
    writer2 << (*pCore2);
    bytevector state2;
    writer2.CopyTo(state2);

    // Verify
    ASSERT_EQ(state1.size(), state2.size());

    for (size_t i = 0; i < state1.size(); i++)
    {
        ASSERT_EQ(state1[i], state2[i]);
    }
}

TEST(CoreTests, RunUntilVSync)
{
    // Setup
    std::unique_ptr<Core> pCore = std::make_unique<Core>();
    qword ticksEnd = pCore->Ticks() + 4000000;
    int vSyncCount = 0;

    // Act
    while (pCore->Ticks() < ticksEnd)
    {
        pCore->RunUntil(ticksEnd, stopVSync, nullptr);
        vSyncCount++;
    }

    // Verify - in one second, we should have 50 or 51 VSync's, roughly in line with a 50Hz refresh rate.
    ASSERT_GE(vSyncCount, 50);
    ASSERT_LE(vSyncCount, 51);
}

TEST(CoreTests, KeyPress)
{
    // Setup
    std::unique_ptr<Core> pCore = std::make_unique<Core>();

    // Act
    bool prevdown1 = pCore->KeyPress(65, true);
    bool prevdown2 = pCore->KeyPress(65, true);
    bool prevdown3 = pCore->KeyPress(65, false);

    // Verify
    ASSERT_TRUE(prevdown1);
    ASSERT_FALSE(prevdown2);
    ASSERT_TRUE(prevdown3);
}

// Ensures that for 8- and 16-bit registers declared in a "union/struct" fashion, the physical addresses
// of each register is as expected (i.e. the physical address of the 16-bit register is the same as the
// lower 8-bit register, and the upper 8-bit register immediately follows the lower). Adding this test as
// I'm paranoid that struct member alignments might unexpectedly change...
TEST(CoreTests, CheckAlignments)
{
    // Setup
    std::unique_ptr<Core> pCore = std::make_unique<Core>();

    // Verify
    ASSERT_EQ(&pCore->F, (byte*)&pCore->AF);
    ASSERT_EQ(&pCore->C, (byte*)&pCore->BC);
    ASSERT_EQ(&pCore->E, (byte*)&pCore->DE);
    ASSERT_EQ(&pCore->L, (byte*)&pCore->HL);
    ASSERT_EQ(&pCore->R, (byte*)&pCore->IR);

    ASSERT_EQ(&pCore->F + 1, &pCore->A);
    ASSERT_EQ(&pCore->C + 1, &pCore->B);
    ASSERT_EQ(&pCore->E + 1, &pCore->D);
    ASSERT_EQ(&pCore->L + 1, &pCore->H);
    ASSERT_EQ(&pCore->R + 1, &pCore->I);
}
