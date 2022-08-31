#include <sstream>
#include "documentWrite2d.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/imgdoc2::dbIndex DocumentWrite2d::AddTile(
    const imgdoc2::ITileCoordinate* coord,
    const imgdoc2::LogicalPositionInfo* info,
    const imgdoc2::TileBaseInfo* tileInfo,
    imgdoc2::DataTypes datatype,
    const imgdoc2::IDataObjBase* data)
{
    auto tiles_data_id = this->AddTileData(tileInfo->pixelWidth, tileInfo->pixelHeight, tileInfo->pixelType, datatype);

    ostringstream ss;
    ss << "INSERT INTO " << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " ("
        << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << ","
        << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << ","
        << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << ","
        << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << ","
        << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) << ","
        << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId);

    vector<int> coordinate_values;
    coord->EnumCoordinates(
        [&](Dimension d, int v)->bool
        {
            ss << ", " << this->document_->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << d;
            coordinate_values.push_back(v);
            return true;
        });

    ss << ") VALUES( ?, ?, ?, ?, ?, ?";
    for (size_t i = 0; i < coordinate_values.size(); ++i)
    {
        ss << ", ?";
    }

    ss << ");";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(ss.str());
    statement->BindDouble(1, info->posX);
    statement->BindDouble(2, info->posY);
    statement->BindDouble(3, info->width);
    statement->BindDouble(4, info->height);
    statement->BindInt32(5, info->pyrLvl);
    statement->BindInt64(6, tiles_data_id);

    int i = 0;
    for (const int v : coordinate_values)
    {
        statement->BindInt32(i + 7, v);
        i++;
    }

    auto row_id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());

    if (this->document_->GetDataBaseConfiguration2d()->GetIsUsingSpatialIndex())
    {
        this->AddToSpatialIndex(row_id, *info);
    }

    return row_id;
}

imgdoc2::dbIndex DocumentWrite2d::AddTileData(std::uint32_t width, std::uint32_t height, std::uint8_t pixeltype, imgdoc2::DataTypes datatype)
{
    ostringstream ss;
    ss << "INSERT INTO " << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << " ("
        <<"["<< this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelWidth) << "],"
        <<"["<< this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelHeight) << "],"
        <<"["<< this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_PixelType) << "],"
        <<"["<< this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_TileDataType) << "]"
        ") VALUES( ?1, ?2, ?3, ?4);";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(ss.str());
    statement->BindInt32(1, width);
    statement->BindInt32(2, height);
    statement->BindInt32(3, pixeltype);
    statement->BindInt32(4, static_cast<underlying_type_t<decltype(datatype)>>(datatype));

    auto row_id = this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());
    return row_id;
}

void DocumentWrite2d::AddToSpatialIndex(imgdoc2::dbIndex index, const imgdoc2::LogicalPositionInfo& logical_position_info)
{
    ostringstream ss;
    ss << "INSERT INTO " << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesSpatialIndexTableOrThrow() << " ("
        <<"["<<this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << "],"
        <<"["<<this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "],"
        <<"["<<this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << "],"
        <<"["<<this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "],"
        <<"["<<this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << "]"
        ") VALUES(?1,?2,?3,?4,?5);";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(ss.str());
    statement->BindInt64(1, index);
    statement->BindDouble(2, logical_position_info.posX);
    statement->BindDouble(3, logical_position_info.posX + logical_position_info.width);
    statement->BindDouble(4, logical_position_info.posY);
    statement->BindDouble(5, logical_position_info.posY + logical_position_info.height);

    this->document_->GetDatabase_connection()->ExecuteAndGetLastRowId(statement.get());
}