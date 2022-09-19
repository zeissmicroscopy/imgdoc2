namespace ImgDoc2Net
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using ImgDoc2Net.Interop;

    public class CreateOptions : IDisposable
    {
        private IntPtr objectHandle;

        public CreateOptions()
        {
            this.objectHandle = ImgDoc2ApiInterop.Instance.CreateCreateOptions();
        }

        public string Filename
        {
            get { return ImgDoc2ApiInterop.Instance.CreateOptionsGetFilename(this.objectHandle); }
            set { ImgDoc2ApiInterop.Instance.CreateOptionsSetFilename(this.objectHandle, value); }
        }

        public bool UseSpatialIndex
        {
            get { return ImgDoc2ApiInterop.Instance.CreateOptionsGetUseSpatialIndex(this.objectHandle); }
            set { ImgDoc2ApiInterop.Instance.CreateOptionsSetUseSpatialIndex(this.objectHandle, value); }
        }

        private void ReleaseUnmanagedResources()
        {
            ImgDoc2ApiInterop.Instance.DestroyCreateOptions(this.objectHandle);
            this.objectHandle = IntPtr.Zero;
        }

        protected virtual void Dispose(bool disposing)
        {
            ReleaseUnmanagedResources();
            if (disposing)
            {
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        ~CreateOptions()
        {
            Dispose(false);
        }
    }
}
