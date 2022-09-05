#include <variant>
#include <sstream>
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
        for (auto dimension : this->document_->GetDataBaseConfiguration2d()->GetTileDimensions())
        {
            coord->Set(dimension, statement->GetResultInt32(no_of_tile_dimension_columns_in_result));
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

/*virtual*/void DocumentRead2d::Query(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, const std::function<bool(imgdoc2::dbIndex)>& func)
{
    auto query_statement = this->CreateQueryStatement(coordinate_clause, tileinfo_clause);

    while (this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        imgdoc2::dbIndex index = query_statement->GetResultInt64(0);
        const bool continue_operation = func(index);
        if (!continue_operation)
        {
            break;
        }
    }
}

/*virtual*/void DocumentRead2d::GetTilesIntersectingRect(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, std::function<bool(imgdoc2::dbIndex)> func)
{
    shared_ptr<IDbStatement> query_statement;
    if (this->document_->GetDataBaseConfiguration2d()->GetIsUsingSpatialIndex())
    {
        query_statement = this->GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClauseWithSpatialIndex(rect, coordinate_clause, tileinfo_clause);
    }
    else
    {
        query_statement = this->GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClause(rect, coordinate_clause, tileinfo_clause);
    }

    while (this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        imgdoc2::dbIndex index = query_statement->GetResultInt64(0);
        bool continue_operation = func(index);
        if (!continue_operation)
        {
            break;
        }
    }
}

shared_ptr<IDbStatement> DocumentRead2d::GetReadTileInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info)
{
    stringstream string_stream;
    string_stream << "SELECT ";

    if (include_tile_coordinates)
    {
        const auto tile_dimension = this->document_->GetDataBaseConfiguration2d()->GetTileDimensions();
        bool is_first = true;
        for (const auto dimension : tile_dimension)
        {
            if (!is_first)
            {
                string_stream << ",";
            }

            string_stream << "[" << this->document_->GetDataBaseConfiguration2d()->GetDimensionsColumnPrefix() << dimension << "]";
            is_first = false;
        }
    }

    if (include_logical_position_info)
    {
        string_stream << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << "],"
            << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel) << "] ";
    }

    string_stream << "FROM [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << "] "
        << "WHERE [" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "] = ?1;";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    return statement;
}

shared_ptr<IDbStatement> DocumentRead2d::CreateQueryStatement(const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause)
{
    ostringstream string_stream;
    string_stream << "SELECT [" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << "]," <<
        "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileDataId) << "] " <<
        "FROM [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << "] " <<
        "WHERE ";

    auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->document_->GetDataBaseConfiguration2d());
    string_stream << get<0>(query_statement_and_binding_info) << ";";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());

    int binding_index = 1;
    for (const auto& bind_info : get<1>(query_statement_and_binding_info))
    {
        if (holds_alternative<int>(bind_info.value))
        {
            statement->BindInt32(binding_index, get<int>(bind_info.value));
        }
        else if (holds_alternative<int64_t>(bind_info.value))
        {
            statement->BindInt64(binding_index, get<int64_t>(bind_info.value));
        }
        else if (holds_alternative<double>(bind_info.value))
        {
            statement->BindDouble(binding_index, get<double>(bind_info.value));
        }
        else
        {
            throw logic_error("invalid variant");
        }

        ++binding_index;
    }

    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQueryWithSpatialIndex(const imgdoc2::RectangleD& rect)
{
    ostringstream string_stream;
    string_stream << "SELECT " << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << " FROM " <<
        this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesSpatialIndexTableOrThrow() << " WHERE " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << ">=?1 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "<=?2 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << ">=?3 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "<=?4";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindDouble(1, rect.x);
    statement->BindDouble(2, rect.x + rect.w);
    statement->BindDouble(3, rect.y);
    statement->BindDouble(4, rect.y + rect.h);
    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQuery(const imgdoc2::RectangleD& rect)
{
    ostringstream string_stream;
    string_stream << "SELECT " << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << " FROM " <<
        this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " WHERE " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << '+' <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << ">=?1 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "<=?2 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << '+' <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << ">=?3 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "<=?4";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindDouble(1, rect.x);
    statement->BindDouble(2, rect.x + rect.w);
    statement->BindDouble(3, rect.y);
    statement->BindDouble(4, rect.y + rect.h);
    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClauseWithSpatialIndex(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause)
{
    if (coordinate_clause == nullptr && tileinfo_clause == nullptr)
    {
        return this->GetTilesIntersectingRectQueryWithSpatialIndex(rect);
    }

    ostringstream string_stream;
    string_stream << "SELECT spatialindex." << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << " FROM "
        << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesSpatialIndexTableOrThrow() << " spatialindex "
        << "INNER JOIN " << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " info ON "
        << "spatialindex." << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_Pk) << " = info." << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk)
        << " WHERE (" <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxX) << ">=? AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinX) << "<=? AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MaxY) << ">=? AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesSpatialIndexTableOrThrow(DatabaseConfiguration2D::kTilesSpatialIndexTable_Column_MinY) << "<=?) ";

    auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->document_->GetDataBaseConfiguration2d());
    string_stream << " AND " << get<0>(query_statement_and_binding_info) << ";";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindDouble(binding_index++, rect.x);
    statement->BindDouble(binding_index++, rect.x + rect.w);
    statement->BindDouble(binding_index++, rect.y);
    statement->BindDouble(binding_index++, rect.y + rect.h);

    for (const auto& bind_info : get<1>(query_statement_and_binding_info))
    {
        if (holds_alternative<int>(bind_info.value))
        {
            statement->BindInt32(binding_index, get<int>(bind_info.value));
        }
        else if (holds_alternative<int64_t>(bind_info.value))
        {
            statement->BindInt64(binding_index, get<int64_t>(bind_info.value));
        }
        else if (holds_alternative<double>(bind_info.value))
        {
            statement->BindDouble(binding_index, get<double>(bind_info.value));
        }
        else
        {
            throw logic_error("invalid variant");
        }

        ++binding_index;
    }

    return statement;
}

std::shared_ptr<IDbStatement> DocumentRead2d::GetTilesIntersectingRectQueryAndCoordinateAndInfoQueryClause(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause)
{
    if (coordinate_clause == nullptr && tileinfo_clause == nullptr)
    {
        return this->GetTilesIntersectingRectQuery(rect);
    }

    ostringstream string_stream;
    string_stream << "SELECT " << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_Pk) << " FROM "
        << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesInfoOrThrow() << " WHERE (" <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << '+' <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileW) << ">=?1 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileX) << "<=?2 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << '+' <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileH) << ">=?3 AND " <<
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "<=?4";

    auto query_statement_and_binding_info = Utilities::CreateWhereStatement(coordinate_clause, tileinfo_clause, *this->document_->GetDataBaseConfiguration2d());
    string_stream << " AND " << get<0>(query_statement_and_binding_info) << ";";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    int binding_index = 1;
    statement->BindDouble(binding_index++, rect.x);
    statement->BindDouble(binding_index++, rect.x + rect.w);
    statement->BindDouble(binding_index++, rect.y);
    statement->BindDouble(binding_index++, rect.y + rect.h);

    for (const auto& bind_info : get<1>(query_statement_and_binding_info))
    {
        if (holds_alternative<int>(bind_info.value))
        {
            statement->BindInt32(binding_index, get<int>(bind_info.value));
        }
        else if (holds_alternative<int64_t>(bind_info.value))
        {
            statement->BindInt64(binding_index, get<int64_t>(bind_info.value));
        }
        else if (holds_alternative<double>(bind_info.value))
        {
            statement->BindDouble(binding_index, get<double>(bind_info.value));
        }
        else
        {
            throw logic_error("invalid variant");
        }

        ++binding_index;
    }

    return statement;
}