#include <imgdoc2.h>

using namespace  std;
using namespace imgdoc2;

class TestDataObj : public IDataObjBase
{
public:
    virtual void GetData(const void** p, size_t* s) const override
    {
        static const uint8_t data[]{ 1,2,3,4,5,6,7,8 };
        *p = data;
        *s = sizeof(data);
    }
};

static void Test1()
{
    auto create_options = ClassFactory::CreateCreateOptions();
    // create_options->SetFilename(":memory:");
    create_options->SetFilename("d:\\test.db");
    create_options->SetUseSpatialIndex(true);
    create_options->SetCreateBlobTable(true);
    create_options->AddDimension('M');
    create_options->AddIndexForDimension('M');

    auto doc = ClassFactory::CreateNew(create_options);

    auto writer = doc->GetWriter2d();

    writer->BeginTransaction();
    TestDataObj test_data_object;
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
            writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::UNCOMPRESSED_BITMAP, TileDataStorageType::BlobInDatabase, &test_data_object);
        }
    }

    writer->CommitTransaction();
}

static void Test2()
{
    unique_ptr<imgdoc2::IOpenExistingOptions> open_existing_options( ClassFactory::CreateOpenExistingOptions() );
    open_existing_options->SetFilename("D:\\test.db");

    auto doc = ClassFactory::OpenExisting(open_existing_options.get());
}

int main(int argc, char** argv)
{
    //Test1();
    Test2();
    return 0;

    auto create_options = ClassFactory::CreateCreateOptions();
    // create_options->SetFilename(":memory:");
    create_options->SetFilename("d:\\test.db");
    create_options->SetUseSpatialIndex(true);
    create_options->AddDimension('C');
    create_options->AddDimension('Z');

    auto doc = ClassFactory::CreateNew(create_options);

    auto writer = doc->GetWriter2d();

    LogicalPositionInfo position_info;
    TileBaseInfo tileInfo;
    TileCoordinate tc({ { 'C',1234 },{ 'Z',4321 } });
    position_info.posX = 1;
    position_info.posY = 2;
    position_info.width = 3;
    position_info.height = 4;
    position_info.pyrLvl = 0;
    tileInfo.pixelWidth = 100;
    tileInfo.pixelHeight = 101;
    tileInfo.pixelType = 0;
    writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);

    tc.Set('C', 1235);
    writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);
    tc.Set('C', 1236);
    writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, TileDataStorageType::Invalid, nullptr);

    writer.reset();

    auto reader = doc->GetReader2d();

    LogicalPositionInfo position_info_out;
    TileCoordinate tcRead;
    reader->ReadTileInfo(1, &tcRead, &position_info_out);

    CDimCoordinateQueryClause dimension_query_clause;
    dimension_query_clause.AddRangeClause('C', IDimCoordinateQueryClause::RangeClause{ 1233, 1238 });

    vector<dbIndex> resulting_indices;
    reader->Query(
        &dimension_query_clause,
        nullptr,
        [&](dbIndex index)->bool
        {
            resulting_indices.emplace_back(index); return true;
        });

    return 0;
}