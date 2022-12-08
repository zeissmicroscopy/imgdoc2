namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    public struct DimensionCondition
    {
        public Dimension Dimension;
        public int RangeStart;
        public int RangeEnd;
    }

    public interface IDimensionQueryClause
    {
        IEnumerable<DimensionCondition> EnumConditions();
    }
}
