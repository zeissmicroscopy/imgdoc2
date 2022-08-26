#include "documentRead2d.h"

using namespace std;

/*virtual*/void DocumentRead2d::ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info)
{
    const auto statement = this->GetReadTileInfo_Statement(coord != nullptr, info != nullptr);
    statement->BindInt64(1, idx);

    this->document_->GetDatabase_connection()->StepStatement(statement.get());

    int no_of_tile_dimension_columns_in_result = 0;
    if (coord != nullptr)
    {
        coord->Clear();
        for (auto d : this->document_->GetDataBaseConfiguration2d()->GetTileDimensions())
        {
            coord->Set(d, statement->GetResultInt32(no_of_tile_dimension_columns_in_result));
            no_of_tile_dimension_columns_in_result++;
        }
    }

    if (info != nullptr)
    {
        info->posX = statement->GetResultDouble(0 + no_of_tile_dimension_columns_in_result);
        info->posY = statement->GetResultDouble(1 + no_of_tile_dimension_columns_in_result);
        info->width = statement->GetResultDouble(2 + no_of_tile_dimension_columns_in_result);
        info->height = statement->GetResultDouble(3 + no_of_tile_dimension_columns_in_result);
        info->pyrLvl = statement->GetResultInt32(4 + no_of_tile_dimension_columns_in_result);
    }
}

shared_ptr<IDbStatement> DocumentRead2d::GetReadTileInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info)
{
    stringstream ss;
    ss << "SELECT ";

    if (include_tile_coordinates)
    {
        auto tile_dimension = this->document_->GetDataBaseConfiguration2d()->GetTileDimensions();
        bool is_first = true;
        for (auto d : tile_dimension)
        {
            if (!is_first)
            {
                ss << ",";
            }

            ss << "[" << this->document_->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << d << "]";
            is_first = false;
        }
    }

    if (include_logical_position_info)
    {
        ss << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) << "] ";
    }

    ss << "FROM [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << "] "
        << "WHERE [" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "] = ?1;";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(ss.str());
    return statement;
}