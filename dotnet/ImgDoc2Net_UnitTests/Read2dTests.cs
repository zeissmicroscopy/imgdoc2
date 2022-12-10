
namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;
    using System;
    using System.Reflection.Metadata;

    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class Read2dTests
    {
        [Fact]
        public void ReadTileDataWithInvalidKeyAndExpectError()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('A'));
            using var document = ImgDoc2Net.Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();

            Assert.Throws<ImgDoc2Exception>(() => reader2d.ReadTileData(12345));
        }

        [Fact]
        public void ReadTileInfoTest()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('X'));
            createOptions.AddDimension(new Dimension('Y'));
            using var document = ImgDoc2Net.Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            LogicalPosition logicalPosition = new LogicalPosition()
            {
                PositionX = 10,
                PositionY = 20,
                Width = 100,
                Height = 111,
                PyramidLevel = 0
            };

            // we put the "x and y"-index into the pixel data here
            var testData = new byte[] { 1, 2, 3 };

            long pkOfAddedTile = writer2d.AddTile(
                new TileCoordinate(new[] { Tuple.Create(new Dimension('X'), 11), Tuple.Create(new Dimension('Y'), 12) }),
                in logicalPosition,
                new Tile2dBaseInfo(1, 1, PixelType.Gray8),
                DataType.UncompressedBitmap,
                testData);

            TileInfoQueryClause tileInfoQueryClause = new TileInfoQueryClause();
            tileInfoQueryClause.PyramidLevelConditionsModifiable.Add(
                new QueryClause()
                {
                    LogicalOperator = QueryLogicalOperator.Invalid,
                    ComparisonOperator = QueryComparisonOperator.Equal,
                    Value = 0
                });

            var keys = reader2d.Query(null, tileInfoQueryClause, null);

            var tileInfo = reader2d.ReadTileInfo(keys[0]);
        }
    }
}
