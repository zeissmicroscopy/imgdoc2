#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../libimgdoc2/inc/imgdoc2.h"

using namespace std;
using namespace imgdoc2;
using namespace testing;

/// Creates a new in-memory document with the following characteristics: We have 10x10 tiles,
/// each width=height=10, in a checkerboard-arrangement of 10 row and 10 columns. Each tile
/// has an M-index, starting to count from 1.
/// \param  use_spatial_index   True if the document is to use a spatial index.
/// \returns                    The newly created in-memory "checkerboard document".
shared_ptr<IDoc> CreateCheckerboardDocument(bool use_spatial_index)
{
    auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    //create_options->SetFilename("d:\\test.db");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(true);
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options.get());
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
            writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
        }
    }

    return doc;
}

/// Utility for retrieving the M-coordinate from a list of tiles. No error handling is done here.
/// \param [in]     reader          The reader object.
/// \param          keys            The PKs of the tiles to query.
/// \returns        The m index of items.
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

struct WithAndWithoutSpatialIndexFixture1 : public testing::TestWithParam<bool> {};

TEST_P(WithAndWithoutSpatialIndexFixture1, IndexQueryForRectAndCheckResult)
{
    // Using the 10x10 checkerboard-document, we query for tiles overlapping with the ROI (0,0,15,15).
    // We expect to find 4 tiles, with M=1, 2, 11, 12.
    const bool use_spatial_index = GetParam();
    auto doc = CreateCheckerboardDocument(use_spatial_index);
    auto reader = doc->GetReader2d();

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingRect(RectangleD{ 0,0,15,15 },
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index); return true;
        });

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 11, 2, 12));
}

INSTANTIATE_TEST_SUITE_P(
    Query2d,
    WithAndWithoutSpatialIndexFixture1,
    testing::Values(true, false));

/*
TEST(Query2d, WithSpatialIndexQueryForRectAndCheckResult1)
{
    // Using the 10x10 checkerboard-document, we query for tiles overlapping with the ROI (0,0,15,15).
    // We expect to find 4 tiles, with M=1, 2, 11, 12.
    auto doc = CreateCheckerboardDocument(true);
    auto reader = doc->GetReader2d();

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingRect(RectangleD{ 0,0,15,15 },
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index); return true;
        });

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 11, 2, 12));
}

TEST(Query2d, WithoutSpatialIndexQueryForRectAndCheckResult1)
{
    // Using the 10x10 checkerboard-document, we query for tiles overlapping with the ROI (0,0,15,15).
    // We expect to find 4 tiles, with M=1, 2, 11, 12.
    auto doc = CreateCheckerboardDocument(false);
    auto reader = doc->GetReader2d();

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingRect(RectangleD{ 0,0,15,15 },
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index); return true;
        });

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 11, 2, 12));
}
*/

struct WithAndWithoutSpatialIndexFixture2 : public testing::TestWithParam<bool> {};
TEST_P(WithAndWithoutSpatialIndexFixture2, IndexQueryForRectAndCheckResult)
{
    // we use a combined "ROI and coordinate-query", we look for subblocks which intersect with the rectangle (0,0,15,15) and
    // with the M-index in the range 0 to 5 (exclusive the borders), i.e. M > 0 and M < 5.
    // We expect to find two subblocks (with M-index 1 and 2).
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboardDocument(use_spatial_index);
    const auto reader = doc->GetReader2d();

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

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 2));
}

INSTANTIATE_TEST_SUITE_P(
    Query2d,
    WithAndWithoutSpatialIndexFixture2,
    testing::Values(true, false));

struct WithAndWithoutSpatialIndexFixture3 : public testing::TestWithParam<bool> {};

TEST_P(WithAndWithoutSpatialIndexFixture3, IndexQueryForRectAndCheckResult)
{
    // Using the 10x10 checkerboard-document, we query for tiles overlapping with the ROI (0,0,21,21).
    // We expect to find 9 tiles, with M=1, 2, 3, 11, 12, 13, 21, 22, 23. 
    const bool use_spatial_index = GetParam();
    const auto doc = CreateCheckerboardDocument(use_spatial_index);
    const auto reader = doc->GetReader2d();

    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 5 });

    vector<dbIndex> result_indices;
    reader->GetTilesIntersectingRect(RectangleD{ 0,0,21,21 },
        nullptr,
        nullptr,
        [&](dbIndex index)->bool
        {
            result_indices.emplace_back(index); return true;
        });

    const auto m_indices = GetMIndexOfItems(reader.get(), result_indices);
    EXPECT_THAT(m_indices, UnorderedElementsAre(1, 2, 3, 11, 12, 13, 21, 22, 23));
}

INSTANTIATE_TEST_SUITE_P(
    Query2d,
    WithAndWithoutSpatialIndexFixture3,
    testing::Values(true, false));
