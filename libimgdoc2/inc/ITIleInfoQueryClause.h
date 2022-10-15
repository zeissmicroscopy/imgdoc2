#pragma once

#include <functional>
#include <vector>
#include <cstdint>

namespace imgdoc2
{
    enum class LogicalOperator : std::uint8_t
    {
        Invalid = 0,
        And,
        Or,

        MaxValue = Or   ///< This must be equal to the largest numerical value in the enumeration.
    };

    enum class ComparisonOperation : std::uint8_t
    {
        Invalid = 0,
        Equal,
        NotEqual,
        LessThan,
        LessThanOrEqual,
        GreaterThan,
        GreaterThanOrEqual,

        MaxValue = GreaterThanOrEqual  ///< This must be equal to the largest numerical value in the enumeration.
    };

    class ITileInfoQueryClause
    {
    public:
        virtual bool GetPyramidLevelCondition(int no, LogicalOperator* logical_operator, ComparisonOperation* op, int* value) const = 0;
        virtual ~ITileInfoQueryClause() = default;
    };
}
