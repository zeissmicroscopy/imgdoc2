namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using ImgDoc2Net.Implementation;

    /// <summary>   
    /// This interface is used to give read-access to the document. 
    /// </summary>
    public interface IRead2d : IDisposable
    {
        List<long> Query(IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions = null);

        byte[] ReadTileData(long key);

        List<long> QueryTilesIntersectingRect(Rectangle rectangle, IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions = null);
    }
}
