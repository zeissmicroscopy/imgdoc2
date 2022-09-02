#include "utilities.h"

#include <sstream>

using namespace std;
using namespace imgdoc2;

/*static*/std::tuple<std::string, std::vector<Utilities::DataBindInfo>> Utilities::CreateWhereConditionForDimQueryClause(
    const imgdoc2::IDimCoordinateQueryClause* clause,
    const std::function<void(imgdoc2::Dimension, std::string&)>& funcGetColumnNameForDimension)
{
    vector< Utilities::DataBindInfo> databind_info;
    stringstream ss;
    string column_name_for_dimension;
    bool first_dimension_iteration = true;

    for (const auto d : clause->GetTileDimsForClause())
    {
        const std::vector<IDimCoordinateQueryClause::RangeClause>* rangeClauses = clause->GetRangeClause(d);
        // const std::vector<IDimCoordinateQueryClause::RangeClause>* listClause = clause->GetRangeClause(d);

        if (rangeClauses != nullptr)
        {
            if (first_dimension_iteration == false)
            {
                ss << " AND ";
            }

            ss << "(";
            bool first_iteration = true;
            funcGetColumnNameForDimension(d, column_name_for_dimension);
            for (const auto rangeClause : *rangeClauses)
            {
                if (first_iteration == false)
                {
                    ss << " OR ";
                }

                if (rangeClause.start != numeric_limits<int>::min() && rangeClause.end != numeric_limits<int>::max())
                {
                    if (rangeClause.start < rangeClause.end)
                    {
                        ss << "([" << column_name_for_dimension << "] > ? AND [" << column_name_for_dimension << "] < ?)";
                        databind_info.emplace_back(DataBindInfo(rangeClause.start));
                        databind_info.emplace_back(DataBindInfo(rangeClause.end));
                        first_iteration = false;
                    }
                    else if (rangeClause.start == rangeClause.end)
                    {
                        ss << "([" << column_name_for_dimension << "] = ?)";
                        databind_info.emplace_back(DataBindInfo(rangeClause.start));
                        first_iteration = false;
                    }
                }
                else if (rangeClause.start == numeric_limits<int>::min() && rangeClause.end != numeric_limits<int>::max())
                {
                    ss << "([" << column_name_for_dimension << "] < ?)";
                    databind_info.emplace_back(DataBindInfo(rangeClause.end));
                    first_iteration = false;
                }
                else if (rangeClause.start != numeric_limits<int>::min() && rangeClause.end == numeric_limits<int>::max())
                {
                    ss << "([" << column_name_for_dimension << "] > ?)";
                    databind_info.emplace_back(DataBindInfo(rangeClause.start));
                    first_iteration = false;
                }
            }

            ss << ")";

            first_dimension_iteration = false;
        }

    }

    return make_tuple(ss.str(), databind_info);
}

/*static*/std::tuple<std::string, std::vector<Utilities::DataBindInfo>> Utilities::CreateWhereConditionForTileInfoQueryClause(const imgdoc2::ITileInfoQueryClause* clause, const std::string& column_name_pyramidlevel)
{
    ConditionalOperator conditional_operator;
    int value;
    if (!clause->GetPyramidLevelCondition(&conditional_operator, &value))
    {
        return make_tuple(string(), vector<Utilities::DataBindInfo>());
    }

    ostringstream ss;
    ss << "( [" << column_name_pyramidlevel << "] " << Utilities::ConditionalOperatorToString(conditional_operator) << " ?)";
    return make_tuple(ss.str(), vector<Utilities::DataBindInfo>{DataBindInfo(value)});
}

/*static*/const char* Utilities::ConditionalOperatorToString(ConditionalOperator op)
{
    switch (op)
    {
    case ConditionalOperator::Equal:
        return "=";
    case ConditionalOperator::NotEqual:
        return "<>";
    case ConditionalOperator::LessThan:
        return "<";
    case ConditionalOperator::LessThanOrEqual:
        return "<=";
    case ConditionalOperator::GreaterThan:
        return ">";
    case ConditionalOperator::GreaterThanOrEqual:
        return ">=";
    }

    throw invalid_argument("invalid operator encountered");
}

/*static*/std::tuple<std::string, std::vector<Utilities::DataBindInfo>> Utilities::CreateWhereStatement(const imgdoc2::IDimCoordinateQueryClause* dim_coordinate_query_clause, const imgdoc2::ITileInfoQueryClause* tileInfo_query_clause, const DatabaseConfiguration2D& database_configuration)
{
    auto get_column_name_func =
        [&](imgdoc2::Dimension dimension, std::string& column_name)->void
    {
        column_name = database_configuration.GetDimensionsColumnPrefix();
        column_name += dimension;
    };

    if (dim_coordinate_query_clause != nullptr && tileInfo_query_clause != nullptr)
    {
        auto dimension_query = CreateWhereConditionForDimQueryClause(dim_coordinate_query_clause, get_column_name_func);
        auto tileinfo_query = CreateWhereConditionForTileInfoQueryClause(tileInfo_query_clause, database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel));
        ostringstream ss;
        ss << get<0>(dimension_query) << " AND " << get<0>(tileinfo_query);
        auto& databind_info = get<1>(dimension_query);
        std::move(get<1>(tileinfo_query).begin(), get<1>(tileinfo_query).end(), std::back_inserter(databind_info));
        return make_tuple(ss.str(), databind_info);
    }
    else if (dim_coordinate_query_clause != nullptr && tileInfo_query_clause == nullptr)
    {
        return CreateWhereConditionForDimQueryClause(dim_coordinate_query_clause, get_column_name_func);
    }
    else if (dim_coordinate_query_clause == nullptr && tileInfo_query_clause != nullptr)
    {
        return CreateWhereConditionForTileInfoQueryClause(tileInfo_query_clause, database_configuration.GetColumnNameOfTilesInfoTableOrThrow(DatabaseConfiguration2D::kTilesInfoTable_Column_PyramidLevel));
    }

    return make_tuple(" () ", std::vector<Utilities::DataBindInfo>{});
}

/*static*/bool Utilities::TryReadStringFromPropertyBag(IDbConnection* db_connection, const std::string& table_name, const std::string& key_column_name, const std::string& value_column_name, const std::string& key, std::string* output)
{
    ostringstream ss;
    ss << "SELECT [" << value_column_name << "] FROM [" << table_name << "] WHERE [" << key_column_name << "]='" << key << "';";
    const auto statement = db_connection->PrepareStatement(ss.str());
    if (!db_connection->StepStatement(statement.get()))
    {
        return false;
    }
    else
    {
        if (output!=nullptr)
        {
            *output = statement->GetResultString(0);
        }

        return true;
    }
}