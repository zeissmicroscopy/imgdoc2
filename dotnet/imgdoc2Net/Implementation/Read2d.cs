using ImgDoc2Net.Interfaces;
using ImgDoc2Net.Interop;
using System;
using System.Collections.Generic;
using System.Text;

namespace ImgDoc2Net.Implementation
{
    internal partial class Read2d : IRead2d
    {
        private IntPtr reader2dObjectHandle;

        public Read2d(IntPtr handle)
        {
            this.reader2dObjectHandle = handle;
        }
    }

    /// <content>
    /// This part contains the implementation of IDisposable. 
    /// </content>
    internal partial class Read2d
    {
        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            this.ReleaseUnmanagedResources();
            if (disposing)
            {
            }
        }

        private void ReleaseUnmanagedResources()
        {
            ImgDoc2ApiInterop.Instance.DestroyReader2d(this.reader2dObjectHandle);
            this.reader2dObjectHandle = IntPtr.Zero;
        }
    }
}