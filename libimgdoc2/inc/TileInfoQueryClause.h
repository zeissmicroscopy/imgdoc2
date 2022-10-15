#pragma once

#include <vector>
#include <tuple>
#include "ITIleInfoQueryClause.h"

using namespace std;

namespace imgdoc2
{
    class CTileInfoQueryClause : public imgdoc2::ITileInfoQueryClause
    {
    private:
        std::vector<std::tuple< LogicalOperator, ComparisonOperation, int>> pyramid_level_conditionals;
    public:
        CTileInfoQueryClause() = default;

        virtual bool GetPyramidLevelCondition(int no, LogicalOperator* logical_operator, ComparisonOperation* op, int* value) const override
        {
            if (no >= this->pyramid_level_conditionals.size())
            {
                return false;
            }

            const auto& element = this->pyramid_level_conditionals.at(no);
            if (logical_operator != nullptr)
            {
                *logical_operator = get<0>(element);
            }

            if (op != nullptr)
            {
                *op = get<1>(element);
            }

            if (value != nullptr)
            {
                *value = get<2>(element);
            }

            return true;
        }

        void AddPyramidLevelCondition(LogicalOperator logical_operator, ComparisonOperation op, int value)
        {
            this->pyramid_level_conditionals.emplace_back(make_tuple(logical_operator, op, value));
        }
    };
}