namespace ImgDoc2Net.Interop
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Text;

    public partial class ImgDoc2ApiInterop
    {
        private const string BaseDllNameWindows = @"imgdoc2API";
        private const string BaseDllNameLinux = @"libimgdoc2API";

        private static readonly Lazy<ImgDoc2ApiInterop> ImgDoc2ApiInteropInstance = new Lazy<ImgDoc2ApiInterop>(() => new ImgDoc2ApiInterop(), System.Threading.LazyThreadSafetyMode.ExecutionAndPublication);

        /// <summary>   
        /// (Immutable) Win32-Handle of the DLL. If this is IntPtr.Zero, then the loading of the Dll failed (and 'dllLoadErrorMsg' may contain additional information about the problem).
        /// </summary>
        private DllLoader dllLoader;

        private string dllLoadErrorMsg;

        /// <summary>
        /// Prevents a default instance of the <see cref="ImgDoc2ApiInterop"/> class from being created.
        /// </summary>
        /// <remarks>
        /// The strategy of this implementation for dealing with errors (ie. native DLL could not be loaded or some function-exports
        /// could not be found) is:
        /// <list>
        /// <item>
        /// The constructor does NOT throw an exception (ie. the singleton is always available).
        /// </item>
        /// <item>
        /// But, in case of an error-condition, every method will immediately throw an InvalidOperationException exception.
        /// </item>
        /// </list>
        /// </remarks>
        private ImgDoc2ApiInterop()
        {
            var possibleDllFilenames = this.GetFullyQualifiedDllPaths();

            // we try to load all filenames we got, and if "Load" succeeded, we keep this DllLoader-instance
            foreach (var filename in possibleDllFilenames)
            {
                var loader = DllLoader.GetDllLoader(filename);
                try
                {
                    loader.Load();
                    this.dllLoader = loader;
                }
                catch (Exception e)
                {
                    this.dllLoadErrorMsg = e.ToString();
                }
            }

            // it there is no operational DllLoader-instance at this point, we get out of here (and leave this instance in a non-operational state)
            if (this.dllLoader == null)
            {
                return;
            }

            try
            {
                this.createCreateOptions = this.GetProcAddressThrowIfNotFound<VoidAndReturnIntPtrDelegate>("CreateCreateOptions");
                this.destroyCreateOptions = this.GetProcAddressThrowIfNotFound<IntPtrAndReturnVoidDelegate>("DestroyCreateOptions");
                this.createOpenExistingOptions = this.GetProcAddressThrowIfNotFound<VoidAndReturnIntPtrDelegate>("CreateOpenExistingOptions");
                this.destroyOpenExistingOptions = this.GetProcAddressThrowIfNotFound<IntPtrAndReturnVoidDelegate>("DestroyOpenExistingOptions");
                this.createOptionsSetFilename = this.GetProcAddressThrowIfNotFound<CreateOptionsSetFilenameDelegate>("CreateOptions_SetFilename");
                this.createOptionsGetFilename = this.GetProcAddressThrowIfNotFound<CreateOptionsGetFilenameDelegate>("CreateOptions_GetFilename");
            }
            catch (InvalidOperationException exception)
            {
                this.dllLoadErrorMsg = exception.ToString();
                this.dllLoader = null;
            }
        }

        /// <summary>
        /// Gets the (one and only) instance.
        /// </summary>
        /// <value>
        /// The instance.
        /// </value>
        public static ImgDoc2ApiInterop Instance => ImgDoc2ApiInterop.ImgDoc2ApiInteropInstance.Value;

        private static string GetMangledName(string functionName)
        {
            return functionName;
        }

        /// <summary>   
        /// Utility to create a .NET-delegate for the specified exported function. 
        /// If the exported function is not found, then an 'InvalidOperationException' exception is thrown.
        /// </summary>
        /// <typeparam name="T">    The delegate type definition. </typeparam>
        /// <param name="name" type="string">   The name of the exported function. </param>
        /// <returns type="T">  
        /// The newly constructed delegate.
        /// </returns>
        private T GetProcAddressThrowIfNotFound<T>(string name)
            where T : Delegate
        {
            T del = this.TryGetProcAddress<T>(name);
            if (del == null)
            {
                throw new InvalidOperationException($"Function \"{name}\" was not found.");
            }

            return del;
        }

        private T TryGetProcAddress<T>(string name)
            where T : Delegate
        {
            //IntPtr addressOfFunctionToCall = ImgDoc2ApiInterop.GetProcAddress(this.dllHandle, GetMangledName(name));
            IntPtr addressOfFunctionToCall = this.dllLoader.GetProcAddress(GetMangledName(name));
            if (addressOfFunctionToCall == IntPtr.Zero)
            {
                return null;
            }

            return (T)Marshal.GetDelegateForFunctionPointer(addressOfFunctionToCall, typeof(T));
        }

        private void ThrowIfNotInitialized()
        {
            if (this.dllLoader == null)
            {
                throw new InvalidOperationException($"{nameof(ImgDoc2ApiInterop)} is not operational, native dynamic-link-library could not be loaded. Debug-info: {this.dllLoadErrorMsg ?? "<not available>"}");
            }
        }

        private IEnumerable<string> GetFullyQualifiedDllPaths()
        {
            // TODO(JBL): I'd like to have CPU-architecture-specific suffixes for the filenames ("x86", "x64", "arm32" etc.) and probably a "d" for debug-builds or so
            string pathOfExecutable = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            bool isLinux = Utilities.IsLinux();

            if (!isLinux)
            {
                yield return Path.Combine(pathOfExecutable, BaseDllNameWindows + ".dll");
            }
            else
            {
                yield return Path.Combine(pathOfExecutable, BaseDllNameLinux + ".so");
            }
        }
    }

    /// <content>   
    /// In this part we find the wrappers for calling into native code. 
    /// </content>
    public partial class ImgDoc2ApiInterop
    {
        public IntPtr CreateCreateOptions()
        {
            this.ThrowIfNotInitialized();
            return this.createCreateOptions();
        }

        public void DestroyCreateOptions(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();
            this.destroyCreateOptions(handleCreateOptions);
        }

        public void CreateOptionsSetFilename(IntPtr handleCreateOptions, string filename)
        {
            this.ThrowIfNotInitialized();
            if (filename == null)
            {
                throw new ArgumentNullException(nameof(filename), "Must not be null.");
            }

            byte[] bytesUtf8 = Encoding.UTF8.GetBytes(filename);
            unsafe
            {
                fixed (byte* pointerBytesUtf8 = &bytesUtf8[0])
                {
                    this.createOptionsSetFilename(handleCreateOptions, new IntPtr(pointerBytesUtf8));
                }
            }
        }

        public string CreateOptionsGetFilename(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            // TODO(JBL): we are abusing UIntPtr as an equivalent to size_t, c.f. https://stackoverflow.com/questions/32906774/what-is-equal-to-the-c-size-t-in-c-sharp
            UIntPtr sizeOfBuffer = new UIntPtr(1024);
            byte[] buffer = new byte[sizeOfBuffer.ToUInt32()];
            unsafe
            {
                fixed (byte* pointerToBuffer = &buffer[0])
                {
                    // TODO(JBL): quite a lot, error-handling and adapting the buffersize if necessary
                    int returnCode = this.createOptionsGetFilename(handleCreateOptions, new IntPtr(pointerToBuffer), new IntPtr(&sizeOfBuffer));
                }
            }

            var filename = Encoding.UTF8.GetString(buffer, 0, (int)(sizeOfBuffer.ToUInt32() - 1));
            return filename;
        }
    }

    /// <summary>   
    /// This part contains the declaration of the delegates and native structs. 
    /// </summary>
    public partial class ImgDoc2ApiInterop
    {
        private readonly VoidAndReturnIntPtrDelegate createCreateOptions;
        private readonly IntPtrAndReturnVoidDelegate destroyCreateOptions;

        private readonly VoidAndReturnIntPtrDelegate createOpenExistingOptions;
        private readonly IntPtrAndReturnVoidDelegate destroyOpenExistingOptions;

        private readonly CreateOptionsSetFilenameDelegate createOptionsSetFilename;
        private readonly CreateOptionsGetFilenameDelegate createOptionsGetFilename;

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate IntPtr VoidAndReturnIntPtrDelegate();

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IntPtrAndReturnVoidDelegate(IntPtr handle);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptionsSetFilenameDelegate(IntPtr handle, IntPtr fileNameUtf8);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptionsGetFilenameDelegate(IntPtr handle, IntPtr fileNameUtf8, IntPtr size);

    }
}
