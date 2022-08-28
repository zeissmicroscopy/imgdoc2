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

            first_dimension_iteration = false;
        }

    }

    return make_tuple(ss.str(), databind_info);
}
