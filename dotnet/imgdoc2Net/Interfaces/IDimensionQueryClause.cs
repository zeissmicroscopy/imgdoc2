using System;
using System.Collections.Generic;
using System.Text;

namespace ImgDoc2Net.Interfaces
{
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
