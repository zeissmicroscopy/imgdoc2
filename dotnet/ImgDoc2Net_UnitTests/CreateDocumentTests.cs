namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net;
    using ImgDoc2Net.Interop;
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Reflection.Metadata;
    using System.Text;
    using System.Threading.Tasks;


    public class CreateDocumentTests
    {
        [Fact]
        public void CreateNewDocumentAndCheckIfItIsOperational()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            var documentHandle = instance.CreateNewDocument(createOptionsHandle);
            Assert.NotEqual(documentHandle, IntPtr.Zero);
            var reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            Assert.NotEqual(reader2dHandle, IntPtr.Zero);
            instance.DestroyCreateOptions(createOptionsHandle);
            instance.DestroyDocument(documentHandle);
            instance.DestroyReader2d(reader2dHandle);
        }

        [Fact]
        public void CreateNewDocumentWithInvalidCreateOptionsAndExpectException()
        {
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, "../invalid:XXXXXX:");
            Assert.Throws<ImgDoc2Exception>(() => instance.CreateNewDocument(createOptionsHandle));
            instance.DestroyCreateOptions(createOptionsHandle);
        }
    }
}
