using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace ImgDoc2Net.Interop
{
    public partial class ImgDoc2ApiInterop
    {
        //private const string BaseDllName = @"..\..\..\..\out\build\x64-Debug\imgdoc2API\imgdoc2API"; 
        //private const string BaseDllName = @"D:\dev\ZeissGitHub\imgdoc2\out\build\x64-Debug\imgdoc2API\imgdoc2API";
        private const string BaseDllName = @"imgdoc2API";

        private static readonly Lazy<ImgDoc2ApiInterop> ImgDoc2ApiInteropInstance = new Lazy<ImgDoc2ApiInterop>(() => new ImgDoc2ApiInterop(), System.Threading.LazyThreadSafetyMode.ExecutionAndPublication);

        /// <summary>   
        /// (Immutable) Win32-Handle of the DLL. If this is IntPtr.Zero, then the loading of the Dll failed (and 'dllLoadErrorMsg' may contain additional information about the problem).
        /// </summary>
        //private readonly IntPtr dllHandle;
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
            string dllName = ImgDoc2ApiInterop.GetDllName();
            //dllName = "/mnt/d/dev/ZeissGitHub/imgdoc2/dotnet/native_dlls/libimgdoc2API.so";
            dllName = "./libimgdoc2API.so";
            this.dllLoader = DllLoader.GetDllLoader(dllName);
            this.dllLoader.Load();
            //this.dllHandle = this.TryLoadDll(dllName);
            //if (this.dllHandle == IntPtr.Zero)
            //{
            //    return;
            //}

            try
            {
                this.createCreateOptions = this.GetProcAddressThrowIfNotFound<VoidAndReturnIntPtrDelegate>("CreateCreateOptions");
                this.destroyCreateOptions = this.GetProcAddressThrowIfNotFound<IntPtrAndReturnVoidDelegate>("DestroyCreateOptions");
                this.createOpenExistingOptions = this.GetProcAddressThrowIfNotFound<VoidAndReturnIntPtrDelegate>("CreateOpenExistingOptions");
                this.destroyOpenExistingOptions = this.GetProcAddressThrowIfNotFound<IntPtrAndReturnVoidDelegate>("DestroyOpenExistingOptions");
                this.createOptionsSetFilename = this.GetProcAddressThrowIfNotFound<CreateOptionsSetFilenameDelegate>("CreateOptions_SetFilename");
            }
            catch (InvalidOperationException exception)
            {
                //ImgDoc2ApiInterop.FreeLibrary(this.dllHandle);
                this.dllLoadErrorMsg = exception.ToString();
                //this.dllHandle = IntPtr.Zero;
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

        private static string GetDllName()
        {
            var cpuArchitecture = RuntimeInformation.ProcessArchitecture;
            switch (cpuArchitecture)
            {
                case Architecture.X64:
                    return BaseDllName + ".dll";

            }

            return BaseDllName + ".dll";
        }

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

        private IntPtr TryLoadDll(string dllName)
        {
            IntPtr handle = ImgDoc2ApiInterop.LoadLibrary(dllName);
            if (handle == IntPtr.Zero)
            {
                this.dllLoadErrorMsg = $"LoadLibrary failed, GetLastError() is {Marshal.GetLastWin32Error()}";
                return IntPtr.Zero;
            }

            return handle;
        }

        private void ThrowIfNotInitialized()
        {
            //if (this.dllHandle == IntPtr.Zero)
            if (this.dllLoader == null)
            {
                throw new InvalidOperationException($"{nameof(ImgDoc2ApiInterop)} is not operational, DLL \"{ImgDoc2ApiInterop.GetDllName()}\" could not be loaded. Debug-info: {this.dllLoadErrorMsg ?? "<not available>"}");
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

        public unsafe void CreateOptionsSetFilename(IntPtr handleCreateOptions, string filename)
        {
            this.ThrowIfNotInitialized();
            if (filename == null)
            {
                throw new ArgumentNullException(nameof(filename), "Must not be null.");
            }

            byte[] bytesUtf8 = Encoding.UTF8.GetBytes(filename);
            fixed (byte* pointerBytesUtf8 = &bytesUtf8[0])
            {
                this.createOptionsSetFilename(handleCreateOptions, new IntPtr(pointerBytesUtf8));
            }
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

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate IntPtr VoidAndReturnIntPtrDelegate();

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IntPtrAndReturnVoidDelegate(IntPtr handle);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptionsSetFilenameDelegate(IntPtr handle, IntPtr fileNameUtf8);

    }

    /// <content>
    /// Declaration of kernel32-functions LoadLibrary/GetProcAddress et al.
    /// </content>
    public partial class ImgDoc2ApiInterop
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
