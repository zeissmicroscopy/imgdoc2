using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using FluentAssertions;
using ImgDoc2Net;
using ImgDoc2Net.Implementation;
using ImgDoc2Net.Interfaces;

namespace ImgDoc2Net_UnitTests
{
    [Collection(NonParallelCollectionDefinitionClass.Name)]
    public class QueryTests
    {
        [Fact]
        public void CreateDocumentWriteATileReadTileCompareData()
        {
            using var createOptions = new CreateOptions() { Filename = ":memory:", UseBlobTable = true };
            createOptions.AddDimension(new Dimension('A'));
            using var document = ImgDoc2Net.Document.CreateNew(createOptions);
            using var reader2d = document.Get2dReader();
            using var writer2d = document.Get2dWriter();

            List<long> indicesForPyramidLevel3 = new List<long>();
            Random rng = new Random();

            for (int i = 0; i < 100; ++i)
            {
                LogicalPosition logicalPosition = new LogicalPosition()
                {
                    PositionX = 0,
                    PositionY = 1,
                    Width = 2,
                    Height = 3,
                    PyramidLevel = rng.Next(0, 20)
                };
                var testData = new byte[] { 8, 4, 3, 2, 85, 32, 9, 4, 1, 58 };
                long pkOfAddedTile = writer2d.AddTile(
                    new TileCoordinate(new[] { Tuple.Create(new Dimension('A'), i) }),
                    in logicalPosition,
                    new Tile2dBaseInfo(1, 2, PixelType.Gray8),
                    DataType.UncompressedBitmap,
                    testData);
                if (logicalPosition.PyramidLevel == 3)
                {
                    indicesForPyramidLevel3.Add(pkOfAddedTile);
                }
            }

            /*var dimensionQueryClause = new DimensionQueryClause();
            dimensionQueryClause.AddCondition(new DimensionCondition()
            { Dimension = new Dimension('A'), RangeStart = 1, RangeEnd = 1 });*/
            TileInfoQueryClause tileInfoQueryClause = new TileInfoQueryClause();
            tileInfoQueryClause.PyramidLevelConditionsModifiable.Add(
                new QueryClause()
                {
                    LogicalOperator = QueryLogicalOperator.Invalid,
                    ComparisonOperator = QueryComparisonOperator.Equal,
                    Value = 3
                });

            var result = reader2d.Query(null, tileInfoQueryClause);
            // check whether the two lists have the same content, irrespective of order
            indicesForPyramidLevel3.Should().BeEquivalentTo(result);
        }
    }
}
