#pragma once

#include <functional>
#include <vector>

namespace imgdoc2
{
    enum class LogicalOperator
    {
        Invalid,
        And,
        Or,
    };

    enum class ComparisonOperation
    {
        Invalid = 0,
        Equal,
        NotEqual,
        LessThan,
        LessThanOrEqual,
        GreaterThan,
        GreaterThanOrEqual
    };

    class ITileInfoQueryClause
    {
    public:
        virtual bool GetPyramidLevelCondition(int no, LogicalOperator* logical_operator, ComparisonOperation* op, int* value) const = 0;
        virtual ~ITileInfoQueryClause() = default;
    };

    //class ITileInfoQueryClause2
    //{
    //public:
    //    virtual bool 
    //};

}
