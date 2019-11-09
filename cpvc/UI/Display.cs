﻿using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace CPvC
{
    /// <summary>
    /// Small class to deal with copying the screen buffer from a Core to a WriteableBitmap object.
    /// </summary>
    public class Display : IDisposable
    {
        private readonly Int32Rect _drawRect;

        public UnmanagedMemory Buffer { get; private set; }

        public const ushort Width = 768;
        public const ushort Height = 288;
        public const ushort Pitch = Width * sizeof(UInt32);

        public WriteableBitmap Bitmap { get; }

        public Display()
        {
            _drawRect = new Int32Rect(0, 0, Width, Height);

            Buffer = new UnmanagedMemory(Height * Pitch);
            Bitmap = new WriteableBitmap(Width, Height, 0, 0, PixelFormats.Bgr32, BitmapPalettes.Halftone256);
        }

        /// <summary>
        /// Asynchronously copies the display's internal screen buffer to Bitmap. This method ensures this is done on the same thread in which the Bitmap was created.
        /// </summary>
        public void CopyFromBufferAsync()
        {
            Bitmap.Dispatcher.BeginInvoke(new Action(() =>
            {
                CopyFromBuffer();
            }), null);
        }

        /// <summary>
        /// Copies the display's internal screen buffer to Bitmap. Note this method should only be called on the same thread in which the Bitmap was created.
        /// </summary>
        public void CopyFromBuffer()
        {
            if (Buffer != IntPtr.Zero)
            {
                Bitmap.WritePixels(_drawRect, Buffer, Pitch * Height, Pitch);
            }
        }

        /// <summary>
        /// Populates the display from a bookmark.
        /// </summary>
        /// <param name="bookmark">Bookmark to populate the display from.</param>
        public void GetFromBookmark(Bookmark bookmark)
        {
            // Assume a blank screen if no bookmark provided.
            if (bookmark == null)
            {
                Buffer.Clear();
                return;
            }

            // Otherwise, use the bookmark to create a core, and run it for 2 VSync's in order to populate the screen buffer.
            using (Core core = Core.Create(bookmark.State))
            {
                core.SetScreenBuffer(Buffer);
                core.RunForVSync(2);

                CopyFromBuffer();
            }
        }

        public void Dispose()
        {
            Dispose(true);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (Buffer != null)
            {
                Buffer.Dispose();
            }
        }
    }
}
