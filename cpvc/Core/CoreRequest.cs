﻿using System;

namespace CPvC
{
    /// <summary>
    /// Represents a request to the core thread.
    /// </summary>
    public class CoreRequest : CoreActionBase
    {
        public CoreRequest(Types type) : base(type)
        {
        }

        static public CoreRequest Reset()
        {
            CoreRequest request = new CoreRequest(Types.Reset);

            return request;
        }

        static public CoreRequest KeyPress(byte keycode, bool down)
        {
            CoreRequest request = new CoreRequest(Types.KeyPress)
            {
                KeyCode = keycode,
                KeyDown = down
            };

            return request;
        }

        static public CoreRequest RunUntil(UInt64 stopTicks, byte stopReason)
        {
            CoreRequest request = new CoreRequest(Types.RunUntil)
            {
                StopTicks = stopTicks,
                StopReason = stopReason
            };

            return request;
        }

        static public CoreRequest LoadDisc(byte drive, byte[] buffer)
        {
            CoreRequest request = new CoreRequest(Types.LoadDisc)
            {
                Drive = drive,
                MediaBuffer = (buffer != null) ? ((byte[])buffer.Clone()) : null
            };

            return request;
        }

        static public CoreRequest LoadTape(byte[] buffer)
        {
            CoreRequest request = new CoreRequest(Types.LoadTape)
            {
                MediaBuffer = (buffer != null) ? ((byte[])buffer.Clone()) : null
            };

            return request;
        }
    }
}
