#include "FDD.h"

FDD::FDD()
{
    Init();
}

FDD::~FDD()
{
}

void FDD::Init()
{
    _currentSector = 0;
    _currentTrack = 0;

    _hasDisk = false;
    _tempDiskLoaded = false;
    _diskImage = bytevector();
}

void FDD::Eject()
{
    Init();
}

bool FDD::Load(Disk& d, const bytevector& image)
{
    _hasDisk = true;

    _diskImage = image;
    _tempDiskLoaded = false;

    return true;
}

Disk& FDD::GetDisk()
{
    if (!_tempDiskLoaded)
    {
        _tempDisk.LoadDisk(_diskImage.data(), _diskImage.size());
        _tempDiskLoaded = true;
    }

    return _tempDisk;
}


// Floppy Drive functions...
bool FDD::IsReady()
{
    if (!_hasDisk)
    {
        return false;
    }

    return true;
}

bool FDD::Seek(const byte cylinder)
{
    if (!_hasDisk)
    {
        // No disk inserted... can't seek!
        return false;
    }

    for (size_t t = 0; t < GetDisk()._tracks.size(); t++)
    {
        if (GetDisk()._tracks.at(t)._id == cylinder)
        {
            _currentTrack = t;
            return true;
        }
    }

    return false;
}

bool FDD::ReadId(CHRN& chrn)
{
    // Return false if we can't find a good ID...
    if (!_hasDisk)
    {
        return false;
    }

    const Sector& sec = CurrentSector();

    chrn._cylinder = sec._track;
    chrn._head = sec._side;
    chrn._record = sec._id;
    chrn._num = sec._size;

    return true;
}

Track& FDD::CurrentTrack()
{
    for (Track& track : GetDisk()._tracks)
    {
        if (track._id == _currentTrack)
        {
            return track;
        }
    }

    throw;
}

Sector& FDD::CurrentSector()
{
    return CurrentTrack()._sectors[_currentSector];
}

bool FDD::WriteData(
    const byte cylinder,
    const byte head,
    const byte sector,
    const byte numBytes,
    const byte endOfTrack,
    const byte gapLength,
    const byte dataLength,
    byte* pBuffer,
    word bufferSize)
{
    if (!_hasDisk)
    {
        return false;
    }

    Track& t = CurrentTrack();

    _currentSector = 0;

    bool found = false;
    size_t x = 0;
    for (; x < t._sectors.size(); x++)
    {
        const Sector& sec = CurrentSector();

        if (sec._track == cylinder &&
            sec._side == head &&
            sec._id == sector &&
            sec._size == numBytes)
        {
            found = true;
            break;
        }

        _currentSector++;
        if ((size_t)_currentSector >= t._sectors.size())
        {
            _currentSector = 0;
        }
    }

    if (!found)
    {
        return false;
    }

    return true;
}

bool FDD::ReadData(
    const byte cylinder,
    const byte head,
    const byte sector,
    const byte numBytes,
    const byte endOfTrack,
    const byte gapLength,
    const byte dataLength,
    byte*& pBuffer,
    word& bufferSize)
{
    if (!_hasDisk)
    {
        return false;
    }

    Track& t = CurrentTrack();

    bool found = false;
    size_t x = 0;
    for (; x < t._sectors.size(); x++)
    {
        const Sector& sec = CurrentSector();

        if (sec._track == cylinder &&
            sec._side == head &&
            sec._id == sector &&
            sec._size == numBytes)
        {
            found = true;
            break;
        }

        _currentSector++;
        if ((size_t)_currentSector >= t._sectors.size())
        {
            _currentSector = 0;
        }
    }

    if (!found)
    {
        return false;
    }

    Sector& sec = CurrentSector();

    pBuffer = &sec._data.front();
    bufferSize = sec._dataLength;

    return true;
}

void FDD::ReadDataResult(byte& cylinder, byte& head, byte& sector, byte& numBytes)
{
    for (size_t t = 0; t < GetDisk()._tracks.size(); t++)
    {
        Track& track = GetDisk()._tracks.at(t);
        if (track._id == cylinder)
        {
            for (size_t s = 0; s < track._sectors.size(); s++)
            {
                Sector& sec = track._sectors.at(s);
                if (sec._id == sector)
                {
                    return;
                }
            }

            // Get first sector on next cylinder
            cylinder++;
            Track& track2 = GetDisk()._tracks.at(cylinder);
            sector = track2._sectors.at(0)._id;

            for (size_t s = 0; s < track2._sectors.size(); s++)
            {
                Sector& sec = track2._sectors.at(s);
                if (sec._id < sector)
                {
                    sector = sec._id;
                }
            }

            return;
        }
    }
}

byte FDD::GetTrack()
{
    if (!_hasDisk)
    {
        return 0;
    }
    else
    {
        return (byte)_currentTrack;
    }
}

StreamWriter& operator<<(StreamWriter& s, const FDD& fdd)
{
    s << fdd._currentSector;
    s << fdd._currentTrack;

    bool hasDisk = fdd._hasDisk;
    s << hasDisk;

    if (hasDisk)
    {
        s << fdd._diskImage;
    }

    return s;
}

StreamReader& operator>>(StreamReader& s, FDD& fdd)
{
    s >> fdd._currentSector;
    s >> fdd._currentTrack;

    s >> fdd._hasDisk;
    if (fdd._hasDisk)
    {
        s >> fdd._diskImage;
    }

    fdd._tempDiskLoaded = false;

    return s;
}

std::ostream& operator<<(std::ostream& s, const FDD& fdd)
{
    s << "FDD: Current sector: " << (int)fdd._currentSector << std::endl;
    s << "FDD: Current track: " << (int)fdd._currentTrack << std::endl;
    s << "FDD: Has disk: " << (int)fdd._hasDisk << std::endl;
    s << "FDD: Disk image: " << StringifyByteArray(fdd._diskImage) << std::endl;
    s << "FDD: Temp disk loaded: " << fdd._tempDiskLoaded << std::endl;

    return s;
}