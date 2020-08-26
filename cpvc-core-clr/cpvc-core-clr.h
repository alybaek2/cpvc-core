#pragma once

#include "..\cpvc-core\Core.h"

using namespace System;

namespace CPvC {

    public ref class CoreCLR
    {
    private:
        Core* _pCore = new Core();

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
            delete _pCore;
            _pCore = nullptr;
        }

        bool LoadSnapshot(int id)
        {
            return _pCore->LoadSnapshot(id);
        }

        void SaveSnapshot(int id)
        {
            _pCore->SaveSnapshot(id);
        }

        void LoadLowerROM(array<byte>^ lowerRom)
        {
            if (lowerRom->Length != 0x4000)
            {
                throw gcnew ArgumentException("Lower rom size is not 16384 bytes!");
            }

            pin_ptr<byte> pBuffer = &lowerRom[0];
            _pCore->SetLowerRom(CreateMem16k(pBuffer));
        }

        void LoadUpperROM(byte slotIndex, array<byte>^ rom)
        {
            if (rom->Length != 0x4000)
            {
                throw gcnew ArgumentException("Upper rom size is not 16384 bytes!");
            }

            pin_ptr<byte> pBuffer = &rom[0];
            _pCore->SetUpperRom(slotIndex, CreateMem16k(pBuffer));
        }

        byte RunUntil(UInt64 stopTicks, byte stopReason)
        {
            return _pCore->RunUntil(stopTicks, stopReason);
        } 

        void Reset()
        {
            _pCore->Reset();
        }

        void SetScreen(IntPtr pBuffer, UInt16 pitch, UInt16 height, UInt16 width)
        {
            _pCore->SetScreen((byte*)pBuffer.ToPointer(), pitch, height, width);
        }

        IntPtr GetScreen()
        {
            return (IntPtr)_pCore->GetScreen();
        }

        bool KeyPress(byte keycode, bool down)
        {
            return _pCore->KeyPress(keycode, down);
        }

        qword Ticks()
        {
            return _pCore->Ticks();
        }

        void LoadTape(array<byte>^ tapeBuffer)
        {
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
            if (discBuffer == nullptr)
            {
                _pCore->LoadDisc(drive, nullptr, 0);
                return;
            }

            pin_ptr<byte> pDiscBuffer = &discBuffer[0];
            _pCore->LoadDisc(drive, pDiscBuffer, discBuffer->Length);
        }

        int GetAudioBuffers(int samples, array<byte>^ channelA, array<byte>^ channelB, array<byte>^ channelC)
        {
            pin_ptr<byte> ppChannelA = (channelA != nullptr) ? &channelA[0] : nullptr;
            pin_ptr<byte> ppChannelB = (channelB != nullptr) ? &channelB[0] : nullptr;
            pin_ptr<byte> ppChannelC = (channelC != nullptr) ? &channelC[0] : nullptr;

            byte* ppChannels[3] = { ppChannelA, ppChannelB, ppChannelC };

            return _pCore->GetAudioBuffers(samples, ppChannels);
        }

        void AudioSampleFrequency(dword frequency)
        {
            _pCore->SetFrequency(frequency);
        }

        array<byte>^ GetState()
        {
            StreamWriter s;
            s << (*_pCore);

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

            s >> (*_pCore);
        }
    };
}
