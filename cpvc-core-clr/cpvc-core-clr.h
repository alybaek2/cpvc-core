#pragma once

#include <msclr\lock.h>

#include "..\cpvc-core\Core.h"

using namespace System;

namespace CPvC {

    public ref class CoreCLR
    {
    private:
        Core* _pCore = new Core();
        Object^ _lockObject = gcnew Object();

    public:
        CoreCLR()
        {
        }

        ~CoreCLR()
        {
            this->!CoreCLR();
        }

        !CoreCLR()
        {
            msclr::lock l(_lockObject);

            delete _pCore;
            _pCore = nullptr;
        }

        void CreateSnapshot(int id)
        {
            msclr::lock l(_lockObject);

            _pCore->CreateSnapshot(id);
        }

        bool DeleteSnapshot(int id)
        {
            msclr::lock l(_lockObject);

            return _pCore->DeleteSnapshot(id);
        }

        bool RevertToSnapshot(int id)
        {
            msclr::lock l(_lockObject);

            return _pCore->RevertToSnapshot(id);
        }

        void LoadLowerROM(array<byte>^ lowerRom)
        {
            if (lowerRom->Length != 0x4000)
            {
                throw gcnew ArgumentException("Lower rom size is not 16384 bytes!");
            }

            pin_ptr<byte> pBuffer = &lowerRom[0];

            msclr::lock l(_lockObject);
            _pCore->SetLowerRom(CreateMem16k(pBuffer));
        }

        void LoadUpperROM(byte slotIndex, array<byte>^ rom)
        {
            if (rom->Length != 0x4000)
            {
                throw gcnew ArgumentException("Upper rom size is not 16384 bytes!");
            }

            pin_ptr<byte> pBuffer = &rom[0];

            msclr::lock l(_lockObject);
            _pCore->SetUpperRom(slotIndex, CreateMem16k(pBuffer));
        }

        byte RunUntil(UInt64 stopTicks, byte stopReason, System::Collections::Generic::List<UInt16>^ samples)
        {
            wordvector tempSamples;
            tempSamples.reserve(200);

            byte reason = 0;
            {
                msclr::lock l(_lockObject);
                reason = _pCore->RunUntil(stopTicks, stopReason, &tempSamples);
            }

            if (samples != nullptr)
            {
                samples->Capacity += (int)tempSamples.size();

                for (word sample : tempSamples)
                {
                    samples->Add(sample);
                }
            }

            return reason;
        } 

        void Reset()
        {
            msclr::lock l(_lockObject);

            _pCore->Reset();
        }

        void SetScreen(UInt16 pitch, UInt16 height, UInt16 width)
        {
            msclr::lock l(_lockObject);

            _pCore->SetScreen(pitch, height, width);
        }

        void CopyScreen(IntPtr pBuffer, UInt64 size)
        {
            msclr::lock l(_lockObject);

            if (_pCore != nullptr)
            {
                _pCore->CopyScreen((byte*)pBuffer.ToPointer(), size);
            }
        }

        bool KeyPress(byte keycode, bool down)
        {
            msclr::lock l(_lockObject);

            return _pCore->KeyPress(keycode, down);
        }

        qword Ticks()
        {
            msclr::lock l(_lockObject);

            return _pCore->Ticks();
        }

        void LoadTape(array<byte>^ tapeBuffer)
        {
            msclr::lock l(_lockObject);

            if (tapeBuffer == nullptr)
            {
                _pCore->LoadTape(nullptr, 0);
                return;
            }

            pin_ptr<byte> pTapeBuffer = &tapeBuffer[0];
            _pCore->LoadTape(pTapeBuffer, tapeBuffer->Length);
        }

        void LoadDisc(byte drive, array<byte>^ discBuffer)
        {
            msclr::lock l(_lockObject);

            if (discBuffer == nullptr)
            {
                _pCore->LoadDisc(drive, nullptr, 0);
                return;
            }

            pin_ptr<byte> pDiscBuffer = &discBuffer[0];
            _pCore->LoadDisc(drive, pDiscBuffer, discBuffer->Length);
        }

        void AudioSampleFrequency(dword frequency)
        {
            msclr::lock l(_lockObject);

            _pCore->SetFrequency(frequency);
        }

        array<byte>^ GetState()
        {
            StreamWriter s;
            {
                msclr::lock l(_lockObject);
                s << (*_pCore);
            }

            size_t size = s.Size();
            array<byte>^ state = gcnew array<byte>((int)size);
            pin_ptr<byte> ppState = &state[0];
            s.CopyTo(ppState, size);

            return state;
        }

        void LoadState(array<byte>^ state)
        {
            StreamReader s;

            pin_ptr<byte> ppState = &state[0];
            s.SetBuffer(ppState, state->Length);

            msclr::lock l(_lockObject);

            s >> (*_pCore);
        }
    };
}
