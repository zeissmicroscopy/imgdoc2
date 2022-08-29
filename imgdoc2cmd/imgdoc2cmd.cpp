#include <imgdoc2.h>

using namespace  std;
using namespace imgdoc2;

int main(int argc, char** argv)
{
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
    writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, nullptr);

    tc.Set('C', 1235);
    writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, nullptr);
    tc.Set('C', 1236);
    writer->AddTile(&tc, &position_info, &tileInfo, DataTypes::ZERO, nullptr);

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