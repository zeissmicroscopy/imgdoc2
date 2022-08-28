#pragma once

#include <string>
#include <variant>
#include <cstdint>
#include <variant>
#include "IDimCoordinateQueryClause.h"

class Utilities
{
public:
    struct DataBindInfo
    {
        explicit DataBindInfo(int v) : value(v)
        {}

        explicit DataBindInfo(int64_t v) : value(v)
        {}

        explicit DataBindInfo(double v) : value(v)
        {}

        std::variant<int, std::int64_t, double> value;
    };
public:
    static std::tuple<std::string, std::vector<DataBindInfo>> CreateWhereConditionForDimQueryClause(const imgdoc2::IDimCoordinateQueryClause* clause, const std::function<void(imgdoc2::Dimension,std::string&)>& funcGetColumnNameForDimension);
};

