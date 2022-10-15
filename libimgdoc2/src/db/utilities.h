#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include <variant>
#include "IDimCoordinateQueryClause.h"
#include "ITIleInfoQueryClause.h"
#include "database_configuration.h"
#include "IDbConnection.h"

class Utilities
{
public:
    struct DataBindInfo
    {
        explicit DataBindInfo(int v) : value(v)
        {}

        explicit DataBindInfo(std::int64_t v) : value(v)
        {}

        explicit DataBindInfo(double v) : value(v)
        {}

        std::variant<int, std::int64_t, double> value;
    };
public:
    static std::tuple<std::string, std::vector<DataBindInfo>> CreateWhereStatement(const imgdoc2::IDimCoordinateQueryClause* dim_coordinate_query_clause, const imgdoc2::ITileInfoQueryClause* tileInfo_query_clause, const DatabaseConfiguration2D& database_configuration);

    static std::tuple<std::string, std::vector<DataBindInfo>> CreateWhereConditionForDimQueryClause(const imgdoc2::IDimCoordinateQueryClause* clause, const std::function<void(imgdoc2::Dimension,std::string&)>& funcGetColumnNameForDimension);

    static std::tuple<std::string, std::vector<DataBindInfo>> CreateWhereConditionForTileInfoQueryClause(const imgdoc2::ITileInfoQueryClause* clause, const std::string& column_name_pyramidlevel);

    /// Attempts to read from the table with the specified name the value of the column 'value_common_name' from the row
    /// where the value in 'key_column_name' is equal to 'key'. If successful, the string (from column 'value_common_name') is put
    /// into 'output' (if 'output' is non-null) and true is returned. If the key is not found, the method returns false.
    /// It is not checked whether there are multiple hits, in such case an arbitrary hit is returned.
    /// Note that in case of an database-error, this method will throw an exception.
    ///
    /// \param [in]  db_connection       The database connection.
    /// \param       table_name          Name of the table.
    /// \param       key_column_name     Name of the key column.
    /// \param       value_column_name   Name of the value column.
    /// \param       key                 The key to search for.
    /// \param [out] output              If non-null and successful, the result is put here.
    ///
    /// \returns    True if it succeeds, false if it fails.
    [[nodiscard]] static bool TryReadStringFromPropertyBag(IDbConnection* db_connection, const std::string& table_name, const std::string& key_column_name, const std::string& value_column_name, const std::string& key, std::string* output);

    static void WriteStringIntoPropertyBag(IDbConnection* db_connection, const std::string& table_name, const std::string& key_column_name, const std::string& value_column_name, const std::string& key, const std::string& value);

    static void DeleteItemFromPropertyBag(IDbConnection* db_connection, const std::string& table_name, const std::string& key_column_name, const std::string& value_column_name, const std::string& key);
private:
    static const char* ComparisonOperatorToString(imgdoc2::ComparisonOperation comparison_operator);
    static const char* LogicalOperatorToString(imgdoc2::LogicalOperator logical_operator);

    static bool ProcessRangeClause(const std::string& column_name_for_dimension, const imgdoc2::IDimCoordinateQueryClause::RangeClause& rangeClause, std::vector<Utilities::DataBindInfo>& databind_info, std::ostringstream& string_stream);

};

