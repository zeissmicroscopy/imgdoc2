
namespace ImgDoc2Net.Interop
{
    using System;

    internal abstract class DllLoader 
    {
        private string filename;
        private IntPtr dllHandle = IntPtr.Zero;

        public static DllLoader GetDllLoader(string filename)
        {
            if (Utilities.IsLinux())
            {
                return new DllLoaderLinux(filename);
            }

            return new DllLoaderWindows(filename);
        }

        protected IntPtr DllHandle
        {
            get { return this.dllHandle; }
            set { this.dllHandle = value; }
        }

        protected string Filename
        {
            get { return this.filename; }
            set { this.filename = value; }
        }

        public DllLoader(string filename)
        {
            this.filename = filename;
        }

        public void Load()
        {
            if (this.DllHandle != IntPtr.Zero)
            {
                throw new InvalidOperationException("Dynamic Link Library already loaded.");
            }

            var handle = this.LoadDynamicLibrary(this.Filename);
            if (handle == IntPtr.Zero)
            {
                throw new Exception($"Could not load the dynamic link library '{this.Filename}'.");
            }

            this.DllHandle = handle;
        }

        public abstract IntPtr GetProcAddress(string functionName);

        protected abstract IntPtr LoadDynamicLibrary(string filename);

        protected void ThrowIfNotOperational()
        {
            if (this.DllHandle == IntPtr.Zero)
            {
                throw new InvalidOperationException("Dynamic link library is not loaded.");
            }
        }
    }
}
