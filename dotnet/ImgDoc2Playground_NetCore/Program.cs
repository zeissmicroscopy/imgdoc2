// See https://aka.ms/new-console-template for more information
using ImgDoc2Net.Interop;


Console.WriteLine("Hello, World!");
var instance = ImgDoc2ApiInterop.Instance;
var handle = instance.CreateCreateOptions();
instance.CreateOptionsSetFilename(handle, "TESTTEXT");
instance.DestroyCreateOptions(handle);
