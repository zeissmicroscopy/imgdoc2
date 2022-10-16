namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using ImgDoc2Net.Interfaces;
    public class TileInfoQueryClause:ITileInfoQueryClause
    {
        public List<QueryClause> PyramidLevelConditionsModifiable { get; } = new List<QueryClause>();

        public IEnumerable<QueryClause> PyramidLevelConditions => this.PyramidLevelConditionsModifiable;
    }
}
