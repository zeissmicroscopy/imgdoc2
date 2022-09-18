using System;
using System.Collections.Generic;
using System.Text;

namespace ImgDoc2Net.Interop
{
    internal abstract class DllLoader
    {
        private string filename;
        private IntPtr dllHandle = IntPtr.Zero;

        public static DllLoader GetDllLoader(string filename)
        {
            if (IsLinux())
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

        public abstract void Load();

        public abstract IntPtr GetProcAddress(string functionName);

        /// <summary>
        /// Gets a value indicating if the current platform is Linunx.
        /// </summary>
        public static bool IsLinux()
        {
            // http://stackoverflow.com/a/5117005/358336
            int p = (int)Environment.OSVersion.Platform;
            return (p == 4) || (p == 6) || (p == 128);
        }
    }
}
