#include <variant>
#include "documentRead2d.h"
#include "../db/utilities.h"

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

/*virtual*/void DocumentRead2d::Query(const imgdoc2::IDimCoordinateQueryClause* clause, const imgdoc2::ITileInfoQueryClause* tileInfoQuery, const std::function<bool(imgdoc2::dbIndex)>& func)
{
    auto query_statement = this->CreateQueryStatement(clause, tileInfoQuery);

    while (this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        imgdoc2::dbIndex index = query_statement->GetResultInt64(0);
        bool b = func(index);
        if (!b)
        {
            break;
        }
    }
}

shared_ptr<IDbStatement> DocumentRead2d::GetReadTileInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info)
{
    stringstream ss;
    ss << "SELECT ";

    if (include_tile_coordinates)
    {
        const auto tile_dimension = this->document_->GetDataBaseConfiguration2d()->GetTileDimensions();
        bool is_first = true;
        for (const auto d : tile_dimension)
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

shared_ptr<IDbStatement> DocumentRead2d::CreateQueryStatement(const imgdoc2::IDimCoordinateQueryClause* clause, const imgdoc2::ITileInfoQueryClause* tileInfoQuery)
{
    ostringstream ss;
    ss << "SELECT [" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << "]," <<
        "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "] " <<
        "FROM [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << "] " <<
        "WHERE ";

    auto query_statement_and_binding_info = Utilities::CreateWhereStatement(clause, tileInfoQuery, *this->document_->GetDataBaseConfiguration2d().get());
    ss << get<0>(query_statement_and_binding_info);

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(ss.str());
    int i = 1;
    for (const auto& bind_info : get<1>(query_statement_and_binding_info))
    {
        if (holds_alternative<int>(bind_info.value))
        {
            statement->BindInt32(i, get<int>(bind_info.value));
        }
        else if (holds_alternative<int64_t>(bind_info.value))
        {
            statement->BindInt64(i, get<int64_t>(bind_info.value));
        }
        else if (holds_alternative<double>(bind_info.value))
        {
            statement->BindDouble(i, get<double>(bind_info.value));
        }
        else
        {
            throw logic_error("invalid variant");
        }

        ++i;
    }

    return statement;
}
