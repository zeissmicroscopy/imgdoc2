

namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    /// <summary>   
    /// This interface is used to give write-access to the document. 
    /// </summary>
    public interface IWrite2d : IDisposable
    {
        long AddTile(
            ITileCoordinate tileCoordinate,
            in LogicalPosition logicalPosition,
            Tile2dBaseInfo tile2dBaseInfo,
            DataType dataType,
            IntPtr pointerTileData,
            long sizeTileData);
    }
}
