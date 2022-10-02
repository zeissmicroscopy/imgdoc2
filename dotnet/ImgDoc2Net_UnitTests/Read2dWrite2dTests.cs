namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using System;

    public class Read2dWrite2dTests
    {
        [Fact]
        public void CreateDocumentWriteATileReadTileCompareData()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('A'));
            using var document = ImgDoc2Net.Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            LogicalPosition logicalPosition = new LogicalPosition() { PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0 };
            var testData = new byte[] { 8, 4, 3, 2, 85, 32, 9, 4, 1, 58 };
            long pkOfAddedTile = writer2d.AddTile(
                new TileCoordinate(new[] { Tuple.Create(new Dimension('A'), 1) }),
                in logicalPosition,
                new Tile2dBaseInfo(1, 2, PixelType.Gray8),
                DataType.Zero,
                testData);

            var dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });

            var result = reader2d.Query(dimensionQueryClause);
            Assert.Single(result);

            Assert.Equal(result[0], pkOfAddedTile);

            var blob = reader2d.ReadTileData(result[0]);

            Assert.Equal(testData.Length, blob.Length);
            Assert.Equal(testData, blob);
        }
    }
}
