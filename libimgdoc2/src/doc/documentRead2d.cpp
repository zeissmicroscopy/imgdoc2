#include <variant>
#include <sstream>
#include "documentRead2d.h"
#include "../db/utilities.h"

using namespace std;
using namespace imgdoc2;

/*virtual*/void DocumentRead2d::ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info)
{
    const auto query_statement = this->GetReadTileInfo_Statement(coord != nullptr, info != nullptr);
    query_statement->BindInt64(1, idx);

    // we are expecting exactly one result, or zero in case of "not found"
    if (!this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        // this means that the tile with the specified index ('idx') was not found
        ostringstream ss;
        ss << "Request for reading tileinfo for an non-existing tile (with pk=" << idx << ")";
        throw non_existing_tile_exception(ss.str(), idx);
    }

    int no_of_tile_dimension_columns_in_result = 0;
    if (coord != nullptr)
    {
        coord->Clear();
        for (auto dimension : this->document_->GetDataBaseConfiguration2d()->GetTileDimensions())
        {
            coord->Set(dimension, query_statement->GetResultInt32(no_of_tile_dimension_columns_in_result));
            no_of_tile_dimension_columns_in_result++;
        }
    }

    if (info != nullptr)
    {
        info->posX = query_statement->GetResultDouble(0 + no_of_tile_dimension_columns_in_result);
        info->posY = query_statement->GetResultDouble(1 + no_of_tile_dimension_columns_in_result);
        info->width = query_statement->GetResultDouble(2 + no_of_tile_dimension_columns_in_result);
        info->height = query_statement->GetResultDouble(3 + no_of_tile_dimension_columns_in_result);
        info->pyrLvl = query_statement->GetResultInt32(4 + no_of_tile_dimension_columns_in_result);
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
        const imgdoc2::dbIndex index = query_statement->GetResultInt64(0);
        const bool continue_operation = func(index);
        if (!continue_operation)
        {
            break;
        }
    }
}

/*virtual*/void DocumentRead2d::ReadTileData(imgdoc2::dbIndex idx, imgdoc2::IBlobOutput* data)
{
    // TODO: if following the idea of a "plug-able blob-storage component", then this operation would be affected.
    shared_ptr<IDbStatement> query_statement = this->GetReadDataQueryStatement(idx);

    // TODO: - we expect one and only one result, should raise an error otherwise
    //       - also, we need to report if no result is found
    if (this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        query_statement->GetResultBlob(0, data);
    }
    else
    {
        // this means that the tile with the specified index ('idx') was not found
        ostringstream ss;
        ss << "Request for reading tiledata for an non-existing tile (with pk=" << idx << ")";
        throw non_existing_tile_exception(ss.str(), idx);

    }

    // if we found multiple "blobs" with above query, this is a fatal error
    if (this->document_->GetDatabase_connection()->StepStatement(query_statement.get()))
    {
        ostringstream ss;
        ss << "Multiple results from 'ReadTileData'-query, which must not happen.";
        this->GetHostingEnvironment()->ReportFatalErrorAndExit(ss.str().c_str());
    }
}

shared_ptr<IDbStatement> DocumentRead2d::GetReadTileInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info)
{
    stringstream string_stream;
    string_stream << "SELECT ";

    bool tile_coordinates_added = false;
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
            tile_coordinates_added = true;
        }
    }

    if (include_logical_position_info)
    {
        if (tile_coordinates_added)
        {
            string_stream << ',';
        }

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
        this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_TileY) << "<=?4)";

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

std::shared_ptr<IDbStatement> DocumentRead2d::GetReadDataQueryStatement(imgdoc2::dbIndex idx)
{
    // we create a statement like this:
    // SELECT [BLOBS].[Data]
    //    FROM [TILESDATA]
    //        LEFT JOIN[BLOBS] ON [TILESDATA].[BinDataId] = [BLOBS].[Pk]
    //        WHERE [TILESDATA].[Pk] = ?1;
    //
    // To be noted: 
    // * If the row with the specified primary key is not found (in the TILESDATA-table), then we
    //    get an empty result set.
    // * If, on the other hand, the row in TILESDATA is found, but there is no corresponding element in the
    //    [BLOBS]-table, then we a result with a null
    // 
    // This allows us to distinguish between "invalid idx" and "no blob present"

    ostringstream string_stream;
    string_stream << "SELECT [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "]."
        << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Data) << "] "
        << "FROM [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "] LEFT JOIN [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "] "
        << "ON [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "].[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataId) << "]"
        << " = [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "].[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Pk) << "]"
        << " WHERE [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "].[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_Pk) << "] = ?1;";

    //// we create a statement like this:
    //// 
    //// SELECT [Data] FROM [BLOBS] WHERE [BLOBS].[Pk] =
    ////    (
    ////        SELECT BinDataId FROM TILESDATA WHERE Pk = 1  AND BinDataStorageType = 1
    ////    )
    ////
    ////
    ////
    //
    //ostringstream string_stream;
    //string_stream << "SELECT [" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Data) << "] "
    //    << "FROM [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "] WHERE [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForBlobTableOrThrow() << "].[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfBlobTableOrThrow(DatabaseConfiguration2D::kBlobTable_Column_Pk) << "] = ("
    //    << "SELECT [" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataId) << "] "
    //    << "FROM [" << this->document_->GetDataBaseConfiguration2d()->GetTableNameForTilesDataOrThrow() << "] WHERE ["
    //    << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_Pk) << "] = ?1 AND "
    //    << "[" << this->document_->GetDataBaseConfiguration2d()->GetColumnNameOfTilesDataTableOrThrow(DatabaseConfiguration2D::kTilesDataTable_Column_BinDataStorageType) << "] = ?2"
    //    << ")";

    auto statement = this->document_->GetDatabase_connection()->PrepareStatement(string_stream.str());
    statement->BindInt64(1, idx);
    //    statement->BindInt32(binding_index++, static_cast<std::underlying_type_t<TileDataStorageType>>(TileDataStorageType::BlobInDatabase));
    return statement;

}