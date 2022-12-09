﻿namespace ImgDoc2Net.Interop
{
    using System;

    /// <summary>   
    /// This abstract class is used to handle loading a native DLL.
    /// </summary>
    internal abstract class DllLoader 
    {
        private string filename;
        private IntPtr dllHandle = IntPtr.Zero;

        /// <summary>   
        /// Initializes a new instance of the <see cref="DllLoader"/> class.
        /// Specialized constructor for use only by derived class. Here we store the filename of the dynamic library to be loaded. 
        /// </summary>
        /// <param name="filename" type="string">   Filename of the file. </param>
        protected DllLoader(string filename)
        {
            this.filename = filename;
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

        /// <summary>   
        /// Gets an DLL-loader-instance (appropriate for the platform). 
        /// </summary>
        /// <param name="filename" type="string">   Filename of the file. </param>
        /// <returns type="DllLoader">  The newly created DLL-loader instance. </returns>
        public static DllLoader GetDllLoader(string filename)
        {
            if (Utilities.IsLinux())
            {
                return new DllLoaderLinux(filename);
            }

            return new DllLoaderWindows(filename);
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
