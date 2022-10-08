namespace ImgDoc2Net.Interfaces
{
    using ImgDoc2Net.Implementation;
    using System;
    using System.Collections.Generic;
    using System.Text;

    /// <summary>   
    /// This interface is used to give read-access to the document. 
    /// </summary>
    public interface IRead2d : IDisposable
    {
        List<long> Query(IDimensionQueryClause queryClause);

        byte[] ReadTileData(long key);
    }
}
