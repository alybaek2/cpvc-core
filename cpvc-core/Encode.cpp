#include "Encode.h"

int FindNextRunOfSameBytes(byte* pBytes, int size, int start, int minRunLength, int& runLength)
{
    int runstart = start;
    int i = start + 1;
    int length = 1;
    bool same = true;
    while (i < size)
    {
        if (pBytes[i] == pBytes[runstart])
        {
            i++;
        }
        else
        {
            // Big enough to report as a run?
            if ((i - runstart) >= 5)
            {
                runLength = i - runstart;
                return runstart;
            }

            runstart = i;
            same = true;
        }
    }

    if ((i - runstart) >= 5)
    {
        runLength = i - runstart;
        return runstart;
    }

    // None found
    runLength = 0;
    return -1;
}

void AddRun(byte* pBytes, bytevector& encoded, int start, int end, bool same)
{
    if (same)
    {
        encoded.push_back(0x01); // Use run length coding!
        dword len = (dword)(end - start + 1);
        byte* pLen = (byte*)&len;
        encoded.push_back(pLen[0]);
        encoded.push_back(pLen[1]);
        encoded.push_back(pLen[2]);
        encoded.push_back(pLen[3]);
        encoded.push_back(pBytes[start]);
    }
    else
    {
        encoded.push_back(0x00); // Don't use run length coding!
        dword len = (dword)(end - start + 1);
        byte* pLen = (byte*)&len;
        encoded.push_back(pLen[0]);
        encoded.push_back(pLen[1]);
        encoded.push_back(pLen[2]);
        encoded.push_back(pLen[3]);

        for (int j = start; j <= end; j++)
        {
            encoded.push_back(pBytes[j]);
        }
    }
}

bool RunLengthDecode(const bytevector& encodedBytes, bytevector& decoded)
{
    decoded.clear();

    int i = 0;
    while (i < encodedBytes.size())
    {
        byte isRun = encodedBytes[i];
        dword len = *((dword*)&encodedBytes[i + 1]);
        i += 5;
        size_t t = decoded.size();
        decoded.resize(t + len);
        if (isRun == 0)
        {
            memcpy_s(decoded.data() + t, len, &encodedBytes[i], len);

            i += len;
        }
        else
        {
            memset(decoded.data() + t, encodedBytes[i], len);

            i++;
        }
    }

    return true;
}

bool RunLengthEncode(byte* pBuffer, size_t size, bytevector& encoded)
{
    if (size == 0)
    {
        return false;
    }

    encoded.clear();

    int start = 0;
    byte b = pBuffer[0];
    bool runFound = true;

    int runLength = 0;

    int i = 0;
    while (i < size)
    {
        int runIndex = FindNextRunOfSameBytes(pBuffer, size, i, 5, runLength);
        if (runIndex == -1)
        {
            // Can't find one. Just add the rest of the buffer!
            AddRun(pBuffer, encoded, i, size - 1, false);
            break;
        }
        else
        {
            AddRun(pBuffer, encoded, i, runIndex - 1, false);
            AddRun(pBuffer, encoded, runIndex, runIndex - 1 + runLength, true);

            i = runIndex + runLength;
        }
    }

    return true;
}

