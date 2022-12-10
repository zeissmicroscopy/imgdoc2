#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../libimgdoc2/inc/imgdoc2.h"

using namespace std;
using namespace imgdoc2;
using namespace testing;

TEST(Read2d, TryReadTileInfoForNonExistentTile)
{
    auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    create_options->AddDimension('M');
    create_options->SetUseSpatialIndex(false);
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options.get());

    auto reader = doc->GetReader2d();

    TileCoordinate tc;
    LogicalPositionInfo logical_position_info;
    EXPECT_THROW(
        reader->ReadTileInfo(1234, &tc, &logical_position_info), 
        imgdoc2::non_existing_tile_exception);
}