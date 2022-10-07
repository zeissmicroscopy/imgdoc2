namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;
    using System;
    using System.Reflection.Metadata;

    public class Read2dWrite2dTests
    {
        [Fact]
        public void CreateDocumentWriteATileReadTileCompareData()
        {
            using var createOptions = new CreateOptions() {Filename = ":memory:", UseBlobTable = true};
            createOptions.AddDimension(new Dimension('A'));
            using var document = ImgDoc2Net.Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            LogicalPosition logicalPosition = new LogicalPosition()
                {PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0};
            var testData = new byte[] {8, 4, 3, 2, 85, 32, 9, 4, 1, 58};
            long pkOfAddedTile = writer2d.AddTile(
                new TileCoordinate(new[] {Tuple.Create(new Dimension('A'), 1)}),
                in logicalPosition,
                new Tile2dBaseInfo(1, 2, PixelType.Gray8),
                DataType.UncompressedBitmap,
                testData);

            var dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition()
                {Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1});

            var result = reader2d.Query(dimensionQueryClause);
            Assert.Single(result);

            Assert.Equal(result[0], pkOfAddedTile);

            var blob = reader2d.ReadTileData(result[0]);

            Assert.Equal(testData.Length, blob.Length);
            Assert.Equal(testData, blob);
        }


        [Fact]
        public void CreateDocumentAndAddTenTilesAndReadTilesAndCompareData()
        {
            using var createOptions = new CreateOptions() {Filename = ":memory:", UseBlobTable = true};
            createOptions.AddDimension(new Dimension('A'));
            using var document = ImgDoc2Net.Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            LogicalPosition logicalPosition = new LogicalPosition()
                {PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0};
            List<long> pkOfAddedTiles = new List<long>(10);
            for (int a = 0; a < 10; ++a)
            {
                var testData = Enumerable.Range(0, 20).Select(i => (byte) (a + i)).ToArray();
                long pkOfAddedTile = writer2d.AddTile(
                    new TileCoordinate(new[] {Tuple.Create(new Dimension('A'), a)}),
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 2, PixelType.Gray8),
                    DataType.UncompressedBitmap,
                    testData);
                pkOfAddedTiles.Add(pkOfAddedTile);
            }

            // now, query for the tiles we just added, and check that we get the same tile (or: its pk) as above
            for (int a = 0; a < 10; ++a)
            {
                var dimensionQueryClause = new DimensionQueryClause();
                dimensionQueryClause.AddCondition(new DimensionCondition()
                    {Dimension = new Dimension('A'), RangeStart = a, RangeEnd = a});
                var result = reader2d.Query(dimensionQueryClause);
                Assert.Single(result);
                Assert.Equal(result[0], pkOfAddedTiles[a]);
            }

            // and finally, read the TileData and check it for correctness
            for (int a = 0; a < 10; ++a)
            {
                var blob = reader2d.ReadTileData(pkOfAddedTiles[a]);
                Assert.Equal(20, blob.Length);
                Assert.Equal(blob, Enumerable.Range(0, 20).Select(i => (byte) (a + i)).ToArray());
            }
        }

        [Fact]
        public void CreateDocumentAndTilesWithInvalidCoordinateAndExpectException()
        {
            using var createOptions = new CreateOptions() {Filename = ":memory:", UseBlobTable = true};
            createOptions.AddDimension(new Dimension('A'));
            using var document = ImgDoc2Net.Document.CreateNew(createOptions);
            using var writer2d = document.Get2dWriter();

            LogicalPosition logicalPosition = new LogicalPosition()
                { PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0 };
            var testData = new byte[] { 1,2,3,4 };

            Assert.Throws<ImgDoc2Exception>(
                () => writer2d.AddTile(
                    new TileCoordinate(new[] { Tuple.Create(new Dimension('Z'), 0) }),
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 2, PixelType.Gray8),
                    DataType.UncompressedBitmap,
                    testData));
        }
    }
}
