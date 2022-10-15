namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    public enum QueryLogicalOperator : byte
    {
        Invalid = 0,
        And,
        Or
    }

    public enum QueryComparisonOperator : byte
    {
        Invalid = 0,
        Equal,
        NotEqual,
        LessThan,
        LessThanOrEqual,
        GreaterThan,
        GreaterThanOrEqual,
    }

    public struct QueryClause
    {
        public QueryLogicalOperator LogicalOperator;
        public QueryComparisonOperator ComparisonOperator;
        public int Value;
    }

    public interface ITileInfoQueryClause
    {
        IEnumerable<QueryClause> PyramidLevelConditions { get; }
    }
}
