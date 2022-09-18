using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Xml.Linq;

namespace ImgDoc2Net.Interop
{
    internal partial class DllLoaderWindows : DllLoader
    {
        public DllLoaderWindows(string filename) : base(filename)
        {
        }

        public override void Load()
        {
            this.DllHandle = DllLoaderWindows.LoadLibrary(this.Filename);
        }

        public override IntPtr GetProcAddress(string functionName)
        {
            IntPtr addressOfFunction = DllLoaderWindows.GetProcAddress(this.DllHandle, functionName);
            return addressOfFunction;
        }
    }

    internal partial class DllLoaderWindows
    {
        /// <content>
        /// Declaration of kernel32-functions LoadLibrary/GetProcAddress et al.
        /// </content>

        [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
        private static extern IntPtr LoadLibrary(string dllToLoad);

        [DllImport("kernel32", CharSet = CharSet.Ansi, SetLastError = true, EntryPoint = "GetProcAddress")]
        private static extern IntPtr GetProcAddress(IntPtr handleModule, string procedureName);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool FreeLibrary(IntPtr hModule);
    }
}
