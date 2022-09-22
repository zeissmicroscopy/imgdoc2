﻿namespace ImgDoc2Net.Interop
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
                this.createOptionsGetUseSpatialIndex = this.GetProcAddressThrowIfNotFound<CreateOptions_GetUseSpatialIndexDelegate>("CreateOptions_GetUseSpatialIndex");
                this.createOptionsSetUseSpatialIndex = this.GetProcAddressThrowIfNotFound<CreateOptions_SetUseSpatialIndexDelegate>("CreateOptions_SetUseSpatialIndex");

                this.createOptionsAddDimension = this.GetProcAddressThrowIfNotFound<CreateOptions_AddDimensionDelegate>("CreateOptions_AddDimension");
                this.createOptionsAddIndexedDimension = this.GetProcAddressThrowIfNotFound<CreateOptions_AddDimensionDelegate>("CreateOptions_AddIndexedDimension");
                this.createOptionsGetDimensions = this.GetProcAddressThrowIfNotFound<CreateOptions_GetDimensionsDelegate>("CreateOptions_GetDimensions");
                this.createOptionsGetIndexedDimensions = this.GetProcAddressThrowIfNotFound<CreateOptions_GetDimensionsDelegate>("CreateOptions_GetIndexedDimensions");
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
                    this.createOptionsSetFilename(handleCreateOptions, new IntPtr(pointerBytesUtf8), null);
                }
            }
        }

        public string CreateOptionsGetFilename(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            const int initialLength = 1024;

            // TODO(JBL): we are abusing UIntPtr as an equivalent to size_t, c.f. https://stackoverflow.com/questions/32906774/what-is-equal-to-the-c-size-t-in-c-sharp
            UIntPtr sizeOfBuffer = new UIntPtr(initialLength);
            byte[] buffer = new byte[sizeOfBuffer.ToUInt32()];
            int returnCode;
            unsafe
            {
                fixed (byte* pointerToBuffer = &buffer[0])
                {
                    returnCode = this.createOptionsGetFilename(handleCreateOptions, new IntPtr(pointerToBuffer), new IntPtr(&sizeOfBuffer), null);
                }

                if (returnCode == ImgDoc2_ErrorCode_OK && sizeOfBuffer.ToUInt32() > initialLength)
                {
                    Array.Resize(ref buffer, (int)sizeOfBuffer.ToUInt32());
                    fixed (byte* pointerToBuffer = &buffer[0])
                    {
                        returnCode = this.createOptionsGetFilename(handleCreateOptions, new IntPtr(pointerToBuffer), new IntPtr(&sizeOfBuffer), null);
                    }
                }
            }

            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                // TODO(Jbl) : stretch out error-handling
                throw new Exception("Error from 'CreateOptionsGetFilename'.");
            }

            var filename = Encoding.UTF8.GetString(buffer, 0, (int)(sizeOfBuffer.ToUInt32() - 1));
            return filename;
        }

        public bool CreateOptionsGetUseSpatialIndex(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();
            bool useSpatialIndex;
            int returnCode;
            unsafe
            {
                returnCode = this.createOptionsGetUseSpatialIndex(handleCreateOptions, &useSpatialIndex, null);
            }

            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                // TODO(Jbl) : stretch out error-handling
                throw new Exception("Error from 'CreateOptionsGetFilename'.");
            }

            return useSpatialIndex;
        }

        public void CreateOptionsSetUseSpatialIndex(IntPtr handleCreateOptions, bool useSpatialIndex)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            unsafe
            {
                returnCode = this.createOptionsSetUseSpatialIndex(handleCreateOptions, useSpatialIndex, null);
            }

            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                // TODO(Jbl) : stretch out error-handling
                throw new Exception("Error from 'CreateOptionsGetFilename'.");
            }
        }

        public void CreateOptionsAddDimension(IntPtr handleCreateOptions, Dimension dimension)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.createOptionsAddDimension(handleCreateOptions, (byte)dimension.Id, &errorInformation);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
        }

        public void CreateOptionsAddIndexedDimension(IntPtr handleCreateOptions, Dimension dimension)
        {
            this.ThrowIfNotInitialized();
            int returnCode;
            ImgDoc2ErrorInformation errorInformation;
            unsafe
            {
                returnCode = this.createOptionsAddIndexedDimension(handleCreateOptions, (byte)dimension.Id, null);
            }

            this.HandleErrorCases(returnCode, in errorInformation);
        }

        public Dimension[] CreateOptionsGetDimensions(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            int returnCode;

            // for sake of simplicity, we only call once into native code with a buffer which is large enough for
            //  all conceivable cases
            const int BufferSize = 64;
            unsafe
            {
                byte* dimensionsBuffer = stackalloc byte[BufferSize];
                UIntPtr sizeOfBuffer = new UIntPtr(BufferSize);  // TODO(Jbl):  we are abusing UIntPtr as an equivalent to size_t, c.f. https://stackoverflow.com/questions/32906774/what-is-equal-to-the-c-size-t-in-c-sharp
                returnCode = this.createOptionsGetDimensions(handleCreateOptions, dimensionsBuffer, new IntPtr(&sizeOfBuffer), null);
                if (sizeOfBuffer.ToUInt32() > BufferSize)
                {
                    throw new NotImplementedException("Buffersize exceeded, may want to implement variable buffersizes.");
                }

                Dimension[] dimensions = new Dimension[sizeOfBuffer.ToUInt32()];
                for (int i = 0; i < sizeOfBuffer.ToUInt32(); i++)
                {
                    dimensions[i] = new Dimension((char)dimensionsBuffer[i]);
                }

                return dimensions;
            }
        }

        public Dimension[] CreateOptionsGetIndexedDimensions(IntPtr handleCreateOptions)
        {
            this.ThrowIfNotInitialized();

            int returnCode;

            // for sake of simplicity, we only call once into native code with a buffer which is large enough for
            //  all conceivable cases
            const int BufferSize = 64;
            unsafe
            {
                byte* dimensionsBuffer = stackalloc byte[BufferSize];
                UIntPtr sizeOfBuffer = new UIntPtr(BufferSize);  // TODO(Jbl):  we are abusing UIntPtr as an equivalent to size_t, c.f. https://stackoverflow.com/questions/32906774/what-is-equal-to-the-c-size-t-in-c-sharp
                returnCode = this.createOptionsGetIndexedDimensions(handleCreateOptions, dimensionsBuffer, new IntPtr(&sizeOfBuffer), null);
                if (sizeOfBuffer.ToUInt32() > BufferSize)
                {
                    throw new NotImplementedException("Buffersize exceeded, may want to implement variable buffersizes.");
                }

                Dimension[] dimensions = new Dimension[sizeOfBuffer.ToUInt32()];
                for (int i = 0; i < sizeOfBuffer.ToUInt32(); i++)
                {
                    dimensions[i] = new Dimension((char)dimensionsBuffer[i]);
                }

                return dimensions;
            }
        }
    }

    public partial class ImgDoc2ApiInterop
    {
        /// <summary> Handles error cases - i.e. if the imgdoc2-API-return-code is indicating an error, we retrieve the
        ///           error-information from the interop-error-information struct and turn it into an appropriate
        ///           .NET exception.</summary>
        /// <param name="returnCode">       The imgdoc2-API-return code.</param>
        /// <param name="errorInformation"> Interop-error-information struct describing the error.</param>
        private void HandleErrorCases(int returnCode, in ImgDoc2ErrorInformation errorInformation)
        {
            if (returnCode != ImgDoc2_ErrorCode_OK)
            {
                string errorMessage;
                unsafe
                {
                    fixed (byte* messagePointerUtf8 = errorInformation.message)
                    {
                        // we need to determine the length of the string (i.e. the position of the terminating '\0') in order
                        // to use 'Encoding.UTF8.GetString' for proper operation. .NET 7 seems to have 'Marshal.PtrToStringUtf8' which
                        // would do the job better.
                        int lengthOfUtf8String;
                        for (lengthOfUtf8String = 0; lengthOfUtf8String < ImgDoc2ErrorInformationMessageMaxLength; ++lengthOfUtf8String)
                        {
                            if (messagePointerUtf8[lengthOfUtf8String] == 0)
                            {
                                break;
                            }
                        }

                        errorMessage = Encoding.UTF8.GetString(messagePointerUtf8, lengthOfUtf8String);
                    }
                }

                throw new ImgDoc2Exception(returnCode, errorMessage);
            }
        }
    }

    /// <summary>   
    /// This part contains the declaration of the delegates and native structs. 
    /// </summary>
    public partial class ImgDoc2ApiInterop
    {
        private const int ImgDoc2_ErrorCode_OK = 0;
        private const int ImgDoc2_ErrorCode_InvalidArgument = 1;
        private const int ImgDoc2_ErrorCode_UnspecifiedError = 50;

        private readonly VoidAndReturnIntPtrDelegate createCreateOptions;
        private readonly IntPtrAndReturnVoidDelegate destroyCreateOptions;

        private readonly VoidAndReturnIntPtrDelegate createOpenExistingOptions;
        private readonly IntPtrAndReturnVoidDelegate destroyOpenExistingOptions;

        private readonly CreateOptionsSetFilenameDelegate createOptionsSetFilename;
        private readonly CreateOptionsGetFilenameDelegate createOptionsGetFilename;
        private readonly CreateOptions_SetUseSpatialIndexDelegate createOptionsSetUseSpatialIndex;
        private readonly CreateOptions_GetUseSpatialIndexDelegate createOptionsGetUseSpatialIndex;
        private readonly CreateOptions_AddDimensionDelegate createOptionsAddDimension;
        private readonly CreateOptions_AddDimensionDelegate createOptionsAddIndexedDimension;
        private readonly CreateOptions_GetDimensionsDelegate createOptionsGetDimensions;
        private readonly CreateOptions_GetDimensionsDelegate createOptionsGetIndexedDimensions;


        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate IntPtr VoidAndReturnIntPtrDelegate();

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int IntPtrAndReturnVoidDelegate(IntPtr handle);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptionsSetFilenameDelegate(IntPtr handle, IntPtr fileNameUtf8, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptionsGetFilenameDelegate(IntPtr handle, IntPtr fileNameUtf8, IntPtr size, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_SetUseSpatialIndexDelegate(IntPtr handle, bool useSpatialIndex, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_GetUseSpatialIndexDelegate(IntPtr handle, bool* useSpatialIndex, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_AddDimensionDelegate(IntPtr handle, byte dim, ImgDoc2ErrorInformation* errorInformation);

        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        private unsafe delegate int CreateOptions_GetDimensionsDelegate(IntPtr handle, byte* dim, IntPtr dimElementCount, ImgDoc2ErrorInformation* errorInformation);

        private const int ImgDoc2ErrorInformationMessageMaxLength = 200;

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        unsafe struct ImgDoc2ErrorInformation
        {
            public fixed byte message[ImgDoc2ErrorInformationMessageMaxLength];
        }
    }
}
