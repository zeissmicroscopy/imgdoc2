namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net;
    using ImgDoc2Net.Implementation;
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
        public void CreateNewDocumentAndCheckIfItIsOperationalAtInteropLevel()
        {
            // this test is operating on "interop"-level
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
        public void CreateNewDocumentWithInvalidCreateOptionsAndExpectExceptionAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, "/../invalid:XXXXXX:");
            Assert.Throws<ImgDoc2Exception>(() => instance.CreateNewDocument(createOptionsHandle));
            instance.DestroyCreateOptions(createOptionsHandle);
        }

        [Fact]
        public void CreateNewDocumentAndCheckIfItIsOperational()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:" };
            using var document = ImgDoc2Net.Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            Assert.NotNull(reader2d);
        }

        [Fact]
        public void CreateNewDocumentAndAddATileAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            var documentHandle = instance.CreateNewDocument(createOptionsHandle);
            Assert.NotEqual(documentHandle, IntPtr.Zero);
            var reader2dHandle = instance.DocumentGetWriter2d(documentHandle);
            Assert.NotEqual(reader2dHandle, IntPtr.Zero);

            TileCoordinate coordinate = new TileCoordinate(
                new[] 
                { 
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('Z'), 2)
                });

            instance.Reader2dAddTile(reader2dHandle, coordinate);

            instance.DestroyCreateOptions(createOptionsHandle);
            instance.DestroyDocument(documentHandle);
            instance.DestroyWriter2d(reader2dHandle);
        }
    }
}
