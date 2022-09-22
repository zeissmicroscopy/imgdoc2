namespace ImgDoc2Net
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using ImgDoc2Net.Interop;

    public partial class CreateOptions : IDisposable
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

        public void AddDimension(Dimension dimension)
        {
            ImgDoc2ApiInterop.Instance.CreateOptionsAddDimension(this.objectHandle, dimension);
        }

        public void AddDimensions(IEnumerable<Dimension> dimensions)
        {
            foreach (var d in dimensions)
            {
                this.AddDimension(d);
            }
        }
        public void AddIndexedDimension(Dimension dimension)
        {
            ImgDoc2ApiInterop.Instance.CreateOptionsAddIndexedDimension(this.objectHandle, dimension);
        }

        public void AddIndexedDimensions(IEnumerable<Dimension> dimensions)
        {
            foreach (var d in dimensions)
            {
                this.AddIndexedDimension(d);
            }
        }

        ~CreateOptions()
        {
            this.Dispose(false);
        }
    }


    /// <content>
    /// This part contains the implementation of IDisposable. 
    /// </content>
    public partial class CreateOptions
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
            ImgDoc2ApiInterop.Instance.DestroyCreateOptions(this.objectHandle);
            this.objectHandle = IntPtr.Zero;
        }
    }
}
