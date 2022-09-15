#include <gtest/gtest.h>
#include "../libimgdoc2_v2/inc/imgdoc2.h"
#include "../libimgdoc2_v2/src/db/utilities.h"

using namespace std;
using namespace imgdoc2;

class BlobDataObj : public IDataObjBase
{
private:
    size_t  size_;
    void* data_;
public:
    BlobDataObj(size_t size) : size_(size)
    {
        this->data_ = malloc(size);
    }

    void GetData(const void** p, size_t* s) const override
    {
        if (p != nullptr)
        {
            *p = this->data_;
        }

        if (s != nullptr)
        {
            *s = this->size_;
        }
    }
};

TEST(BlobData, AddTileWithBlobDataAndRetrieveItCheckForCorrectness)
{
    auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    //create_options->SetFilename("d:\\test.db");
    create_options->AddDimension('M');
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options.get());
    auto writer = doc->GetWriter2d();

    LogicalPositionInfo position_info;
    TileBaseInfo tileInfo;
    TileCoordinate tc({ { 'M',0} });
    position_info.posX = 0;
    position_info.posY = 0;
    position_info.width = 10;
    position_info.height = 10;
    position_info.pyrLvl = 0;
    tileInfo.pixelWidth = 10;
    tileInfo.pixelHeight = 10;
    tileInfo.pixelType = 0;
    BlobDataObj blobData{ 100 };

    writer->AddTile(
        &tc,
        &position_info,
        &tileInfo,
        DataTypes::UNCOMPRESSED_BITMAP,
        TileDataStorageType::BlobInDatabase,
        &blobData);

    auto reader = doc->GetReader2d();
    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 0 });
    vector<dbIndex> indices;
    reader->Query(&coordinate_query_clause, nullptr, [&](dbIndex index)->bool {indices.push_back(index); return true; });
    ASSERT_EQ(indices.size(), 1);
}