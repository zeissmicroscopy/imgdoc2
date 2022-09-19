namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net.Interop;

    public class CreateOptionsTests
    {
        [Fact]
        public void SetFilenameAndGetItAndCompareResult()
        {
            const string Filename = "TESTTEXT";
            var instance = ImgDoc2ApiInterop.Instance;

            var handle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(handle, Filename);

            string s = instance.CreateOptionsGetFilename(handle);

            Assert.Equal(s, Filename);
            instance.DestroyCreateOptions(handle);
        }

        [Fact]
        public void SetFilenameWithUnicodeCharactersAndGetItAndCompareResult()
        {
            const string Filename = "ÄÖÜäöüß 馬匹背負著馬鞍";

            var instance = ImgDoc2ApiInterop.Instance;
            var handle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(handle, Filename);
            string s = instance.CreateOptionsGetFilename(handle);
            Assert.Equal(s, Filename);
            instance.DestroyCreateOptions(handle);
        }
    }
}