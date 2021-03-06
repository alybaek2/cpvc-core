#include "gtest/gtest.h"
#include "../cpvc-core/Keyboard.h"
#include "helpers.h"

TEST(KeyboardTests, InvalidLine) {
    // Setup
    Keyboard keyboard;
    keyboard.SelectLine(11);

    // Act
    bool b = keyboard.KeyPress(11, 0, true);

    // Verify
    ASSERT_EQ(b, false);
}

TEST(KeyboardTests, InvalidBit)
{
    // Setup
    Keyboard keyboard;
    keyboard.SelectLine(0);

    // Act
    bool b = keyboard.KeyPress(0, 8, true);

    // Verify
    ASSERT_EQ(b, false);
}

TEST(KeyboardTests, ReadInvalidLine)
{
    // Setup
    Keyboard keyboard;
    keyboard.SelectLine(10);

    // Act
    byte result = keyboard.ReadSelectedLine();

    // Verify
    ASSERT_EQ(result, 0xff);
}

TEST(KeyboardTests, OneKey)
{
    for (byte line : Range<byte>(0, 9))
    {
        for (byte bit : Range<byte>(0, 7))
        {
            // Setup
            Keyboard keyboard;

            // Act - Test keys "down"
            keyboard.KeyPress(line, bit, true);

            // Verify
            for (byte checkLineDown : Range<byte>(0, 9))
            {
                keyboard.SelectLine(checkLineDown);
                byte matrixLine = keyboard.ReadSelectedLine();
                if (checkLineDown == line)
                {
                    ASSERT_EQ((byte)~(1 << bit), matrixLine);
                }
                else
                {
                    ASSERT_EQ(0xFF, matrixLine);
                }
            }

            // Act - Test keys "up"
            keyboard.KeyPress(line, bit, false);

            // Verify
            for (byte checkLineUp : Range<byte>(0, 9))
            {
                keyboard.SelectLine(checkLineUp);
                ASSERT_EQ(0xFF, keyboard.ReadSelectedLine());
            }
        }
    }
}

TEST(KeyboardTests, TwoKeys)
{
    for (byte line0 : Range<byte>(0, 9))
    {
        for (byte line1 : Range<byte>(0, 9))
        {
            for (byte bit0 : Range<byte>(0, 7))
            {
                for (byte bit1 : Range<byte>(0, 7))
                {
                    // Setup
                    byte expectedMatrix[10];
                    for (int i = 0; i < 10; i++)
                    {
                        expectedMatrix[i] = 0xFF;
                    }

                    expectedMatrix[line0] &= (byte)~(1 << bit0);
                    expectedMatrix[line1] &= (byte)~(1 << bit1);

                    Keyboard keyboard;

                    // Act - Test keys "down"
                    keyboard.KeyPress(line0, bit0, true);
                    keyboard.KeyPress(line1, bit1, true);

                    // Verify
                    for (int i = 0; i < 10; i++)
                    {
                        keyboard.SelectLine(i);
                        ASSERT_EQ(expectedMatrix[i], keyboard.ReadSelectedLine());
                    }

                    // Act - Test keys "up"
                    keyboard.KeyPress(line0, bit0, false);
                    keyboard.KeyPress(line1, bit1, false);

                    // Verify
                    for (byte checkLineUp : Range<byte>(0, 9))
                    {
                        keyboard.SelectLine(checkLineUp);
                        ASSERT_EQ(0xFF, keyboard.ReadSelectedLine());
                    }
                }
            }
        }
    }
};

TEST(KeyboardTests, ThreeKeysClash)
{
    for (byte line0 : Range<byte>(0, 9))
    {
        for (byte line1 : Range<byte>(0, 9))
        {
            if (line1 == line0)
            {
                continue;
            }

            for (byte bit0 : Range<byte>(0, 7))
            {
                for (byte bit1 : Range<byte>(0, 7))
                {
                    if (bit1 == bit0)
                    {
                        continue;
                    }

                    // Setup
                    Keyboard keyboard;

                    // Act - Test that three keys down causes keyboard clash
                    keyboard.KeyPress(line0, bit0, true);
                    keyboard.KeyPress(line0, bit1, true);
                    keyboard.KeyPress(line1, bit0, true);

                    // Verify
                    byte expected = 0xFF & (~(1 << bit0)) & (~(1 << bit1));

                    for (byte checkLine : Range<byte>(0, 9))
                    {
                        keyboard.SelectLine(checkLine);
                        byte matrixLine = keyboard.ReadSelectedLine();
                        if (checkLine == line0 || checkLine == line1)
                        {
                            ASSERT_EQ(expected, matrixLine);
                        }
                        else
                        {
                            ASSERT_EQ(0xFF, matrixLine);
                        }
                    }

                    // Act - Test that one of the three keys going back up removes the clash
                    keyboard.KeyPress(line0, bit0, false);

                    // Verify
                    for (byte checkLineUp : Range<byte>(0, 9))
                    {
                        keyboard.SelectLine(checkLineUp);
                        byte matrixLine = keyboard.ReadSelectedLine();
                        if (checkLineUp == line0)
                        {
                            byte expected = 0xFF & (~(1 << bit1));
                            ASSERT_EQ(expected, matrixLine);
                        }
                        else if (checkLineUp == line1)
                        {
                            byte expected = 0xFF & (~(1 << bit0));
                            ASSERT_EQ(expected, matrixLine);
                        }
                        else
                        {
                            ASSERT_EQ(0xFF, matrixLine);
                        }
                    }
                }
            }
        }
    }
};

TEST(KeyboardTests, Serialize)
{
    // Setup
    byte downKeys[] = { 0, 5, 11, 12, 21, 29, 34, 48, 53, 66, 72 };
    Keyboard keyboard;
    for (byte key : downKeys)
    {
        keyboard.KeyPress(key / 10, key % 10, true);
    }

    keyboard.SelectLine(8);

    // Act
    StreamWriter writer;
    writer << keyboard;

    // Verify
    StreamReader reader(writer);
    Keyboard keyboard2;
    reader >> keyboard2;

    ASSERT_EQ(keyboard.SelectedLine(), keyboard2.SelectedLine());

    for (byte i = 0; i < 9; i++)
    {
        keyboard.SelectLine(i);
        keyboard2.SelectLine(i);
        ASSERT_EQ(keyboard.ReadSelectedLine(), keyboard2.ReadSelectedLine());
    }
}
