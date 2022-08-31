#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../libimgdoc2_a/inc/imgdoc2.h"

using namespace std;
using namespace imgdoc2;
using namespace testing;

shared_ptr<IDoc> CreateCheckerboardDocument()
{
    auto create_options = ClassFactory::CreateCreateOptions();
    create_options->SetFilename(":memory:");
    //create_options->SetFilename("d:\\test.db");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(true);
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options);
    auto writer = doc->GetWriter2d();

    for (int column = 0; column < 10; ++column)
    {
        for (int row = 0; row < 10; ++row)
        {
            LogicalPositionInfo position_info;
            TileBaseInfo tileInfo;
            TileCoordinate tc({ { 'M',column * 10 + row + 1 } });
            position_info.posX = column * 10;
            position_info.posY = row * 10;
            position_info.width = 10;
            position_info.height = 10;
            position_info.pyrLvl = 0;
            tileInfo.pixelWidth = 10; 
            tileInfo.pixelHeight = 10;
            tileInfo.pixelType = 0;
            writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, nullptr);
        }
    }

    return doc;
}

vector<int> GetMIndexOfItems(IDocRead2d* reader, const vector<dbIndex>& keys)
{
    vector<int> m_indices;
    for (auto pk : keys)
    {
        TileCoordinate tc;
        reader->ReadTileInfo(pk, &tc, nullptr);
        int m_index;
        tc.TryGetCoordinate('M', &m_index);
        m_indices.push_back(m_index);
    }

    return m_indices;
}

TEST(Query2d, QueryForRectAndCheckResult1)
{
    auto doc = CreateCheckerboardDocument();
    auto reader = doc->GetReader2d();

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingRect(RectangleD{ 0,0,15,15 },
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index); return true;
        });

    auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 11, 2, 12));
}

TEST(Query2d, QueryForRectAndCheckResult2)
{
    // we use a combined "ROI and coordinate-query", we look for subblocks which intersect with the rectangle (0,0,15,15) and
    // with the M-index in the range 0 to 5 (exclusive the borders), i.e. M > 0 and M < 5.
    // We expect to find two subblocks (with M-index 1 and 2).
    auto doc = CreateCheckerboardDocument();
    auto reader = doc->GetReader2d();

    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 5 });


    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingRect(RectangleD{ 0,0,15,15 },
        &coordinate_query_clause,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index); return true;
        });

    auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 2));
}