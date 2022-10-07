﻿
namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using System;

    public class Read2dTests
    {
        [Fact]
        public void ReadTileDataWithInvalidKeyAndExpectError()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('A'));
            using var document = ImgDoc2Net.Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();

            var result = reader2d.ReadTileData(12345);
        }
    }
}
