namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using ImgDoc2Net.Interfaces;

    public class DimensionQueryClause : IDimensionQueryClause
    {
        private List<DimensionCondition> conditions = new List<DimensionCondition>();

        public void AddCondition(DimensionCondition dimensionCondition)
        {
            this.conditions.Add(dimensionCondition);
        }

        public IEnumerable<DimensionCondition> EnumConditions()
        {
            return this.conditions;
        }
    }
}
