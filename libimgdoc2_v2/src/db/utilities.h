#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include <variant>
#include "IDimCoordinateQueryClause.h"
#include "ITIleInfoQueryClause.h"
#include "database_configuration.h"

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

private:
    static const char* ConditionalOperatorToString(imgdoc2::ConditionalOperator op);
};

