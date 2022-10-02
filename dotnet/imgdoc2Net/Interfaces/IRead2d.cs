namespace ImgDoc2Net.Interfaces
{
    using ImgDoc2Net.Implementation;
    using System;
    using System.Collections.Generic;
    using System.Text;

    public interface IRead2d : IDisposable
    {
        List<long> Query(IDimensionQueryClause queryClause);

        byte[] ReadTileData(long key);
    }
}
