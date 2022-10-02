namespace ImgDoc2Net_UnitTests
{
    using ImgDoc2Net;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
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
        public void CreateNewDocumentAndAddATileAndRunQueriesAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            instance.CreateOptionsSetUseBlobTable(createOptionsHandle, true);
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('A'));
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('Z'));
            var documentHandle = instance.CreateNewDocument(createOptionsHandle);
            Assert.NotEqual(documentHandle, IntPtr.Zero);
            var writer2dHandle = instance.DocumentGetWriter2d(documentHandle);
            Assert.NotEqual(writer2dHandle, IntPtr.Zero);

            // add one tile with coordinate "A1Z2"
            TileCoordinate coordinate = new TileCoordinate(
                new[]
                {
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('Z'), 2)
                });

            LogicalPosition logicalPosition = new LogicalPosition() { PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0 };
            Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(1, 2, PixelType.Gray8);
            instance.Writer2dAddTile(writer2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, DataType.Zero, IntPtr.Zero, 0);

            // now, query for "tiles with A=1" (where there is obviously one)
            var dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });

            var reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            var queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause);

            Assert.True(queryResult.ResultComplete);
            Assert.True(queryResult.Keys.Count == 1, "We expect to find one tile as result of the query.");

            // now, query for "tiles with A=5" (where there is obviously none)
            dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 5, RangeEnd = 5 });

            reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause);
            Assert.True(queryResult.ResultComplete);
            Assert.True(queryResult.Keys.Count == 0, "We expect to find no tile as result of the query.");

            instance.DestroyCreateOptions(createOptionsHandle);
            instance.DestroyDocument(documentHandle);
            instance.DestroyReader2d(reader2dHandle);
            instance.DestroyWriter2d(writer2dHandle);
        }

        [Fact]
        public void CreateNewDocumentAndAddTwoTilesAndRunQueriesAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            instance.CreateOptionsSetUseBlobTable(createOptionsHandle, true);   // TODO(JBl) - since in the following no blob-storage is used, should we allow/ensure operation without blob-store?
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('A'));
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('B'));
            var documentHandle = instance.CreateNewDocument(createOptionsHandle);
            Assert.NotEqual(documentHandle, IntPtr.Zero);
            var writer2dHandle = instance.DocumentGetWriter2d(documentHandle);
            Assert.NotEqual(writer2dHandle, IntPtr.Zero);

            // add one tile with coordinate "A1Z2"
            TileCoordinate coordinate = new TileCoordinate(
                new[]
                {
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('B'), 1)
                });

            LogicalPosition logicalPosition = new LogicalPosition() { PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0 };
            Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(2, 3, PixelType.Gray8);
            instance.Writer2dAddTile(writer2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, DataType.Zero, IntPtr.Zero, 0);

            coordinate = new TileCoordinate(
                new[]
                {
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('B'), 2)
                });

            instance.Writer2dAddTile(writer2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, DataType.Zero, IntPtr.Zero, 0);

            // now, query for "tiles with A=1" (where there are two)
            var dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });

            var reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            var queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause);

            Assert.True(queryResult.ResultComplete);
            Assert.True(queryResult.Keys.Count == 2, "We expect to find two tiles as result of the query.");

            //// now, query for "tiles with A=1 and B=2" (where there is none)
            dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('B'), RangeStart = 2, RangeEnd = 2 });

            reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause);
            Assert.True(queryResult.ResultComplete);
            Assert.True(queryResult.Keys.Count == 1, "We expect to find one tile as result of the query.");

            instance.DestroyCreateOptions(createOptionsHandle);
            instance.DestroyDocument(documentHandle);
            instance.DestroyReader2d(reader2dHandle);
            instance.DestroyWriter2d(writer2dHandle);
        }

        [Fact]
        public void CreateNewDocumentAndAddATileWithTileDataAndReadTileDataBackAndCompareAtInteropLevel()
        {
            // this test is operating on "interop"-level
            var instance = ImgDoc2ApiInterop.Instance;
            var createOptionsHandle = instance.CreateCreateOptions();
            instance.CreateOptionsSetFilename(createOptionsHandle, ":memory:");
            instance.CreateOptionsSetUseBlobTable(createOptionsHandle, true);
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('A'));
            instance.CreateOptionsAddDimension(createOptionsHandle, new Dimension('Z'));
            var documentHandle = instance.CreateNewDocument(createOptionsHandle);
            Assert.NotEqual(documentHandle, IntPtr.Zero);
            var writer2dHandle = instance.DocumentGetWriter2d(documentHandle);
            Assert.NotEqual(writer2dHandle, IntPtr.Zero);

            // add one tile with coordinate "A1Z2"
            TileCoordinate coordinate = new TileCoordinate(
                new[]
                {
                    Tuple.Create(new Dimension('A'), 1) ,
                    Tuple.Create(new Dimension('Z'), 2)
                });

            LogicalPosition logicalPosition = new LogicalPosition() { PositionX = 0, PositionY = 1, Width = 2, Height = 3, PyramidLevel = 0 };
            Tile2dBaseInfo tile2dBaseInfo = new Tile2dBaseInfo(1, 2, PixelType.Gray8);
            byte[] tileData = new byte[5] {1, 2, 3, 4, 5};
            instance.Writer2dAddTile(writer2dHandle, coordinate, in logicalPosition, tile2dBaseInfo, DataType.UncompressedBitmap, tileData);

            // now, query for "tiles with A=1" (where there is obviously one)
            var dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition() { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });

            var reader2dHandle = instance.DocumentGetReader2d(documentHandle);
            var queryResult = instance.Reader2dQuery(reader2dHandle, dimensionQueryClause);

            Assert.True(queryResult.ResultComplete);
            Assert.True(queryResult.Keys.Count == 1, "We expect to find one tile as result of the query.");

            var blob = instance.Reader2dReadTileData(reader2dHandle, queryResult.Keys[0]);
            Assert.True(blob.Length == 5);
            Assert.True(blob[0] == tileData[0] && blob[1] == tileData[1] && blob[2] == tileData[2] && blob[3] == tileData[3] && blob[4] == tileData[4]);

            instance.DestroyCreateOptions(createOptionsHandle);
            instance.DestroyDocument(documentHandle);
            instance.DestroyReader2d(reader2dHandle);
            instance.DestroyWriter2d(writer2dHandle);
        }
    }
}
