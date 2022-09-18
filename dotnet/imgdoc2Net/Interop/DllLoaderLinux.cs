using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace ImgDoc2Net.Interop
{
    internal partial class DllLoaderLinux : DllLoader
    {
        private int importToUse;

        public DllLoaderLinux(string filename) : base(filename)
        {
        }

        public override void Load()
        {
            try
            {
                this.DllHandle = DllLoaderLinux.dlopen1(this.Filename, RTLD_NOW);
                this.importToUse = 1;
            }
            catch (DllNotFoundException)
            {
                this.DllHandle = DllLoaderLinux.dlopen2(this.Filename, RTLD_NOW);
                this.importToUse = 2;
            }
        }

        public override IntPtr GetProcAddress(string functionName)
        {
            IntPtr addressOfFunction;
            addressOfFunction = this.importToUse == 1 ? DllLoaderLinux.dlsym1(this.DllHandle, functionName) : DllLoaderLinux.dlsym2(this.DllHandle, functionName);
            return addressOfFunction;
        }
    }

    internal partial class DllLoaderLinux
    {
        const int RTLD_NOW = 2; // for dlopen's flags 

        [DllImport("libdl.so", EntryPoint = "dlopen")]
        protected static extern IntPtr dlopen1(string filename, int flags);

        [DllImport("libdl.so.2", EntryPoint = "dlopen")]
        protected static extern IntPtr dlopen2(string filename, int flags);

        [DllImport("libdl.so", EntryPoint = "dlsym")]
        protected static extern IntPtr dlsym1(IntPtr handle, string symbol);

        [DllImport("libdl.so.2", EntryPoint = "dlsym")]
        protected static extern IntPtr dlsym2(IntPtr handle, string symbol);

    }
}
