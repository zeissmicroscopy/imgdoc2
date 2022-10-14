

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
        /// <summary>   
        /// Adds a tile with the specified content. 
        /// </summary>
        /// <param name="tileCoordinate" type="ITileCoordinate">        The tile coordinate. </param>
        /// <param name="logicalPosition" type="in LogicalPosition">    The logical position. </param>
        /// <param name="tile2dBaseInfo" type="Tile2dBaseInfo">         Information describing the tile's bitmap. </param>
        /// <param name="dataType" type="DataType">                     The data type (or type if representation) of the pixel data. </param>
        /// <param name="pointerTileData" type="IntPtr">                Pointer to the pixel data. </param>
        /// <param name="sizeTileData" type="long">                     The size of the data (pointed to by "pointerTileData". </param>
        /// <returns type="long">   A long. </returns>
        long AddTile(
            ITileCoordinate tileCoordinate,
            in LogicalPosition logicalPosition,
            Tile2dBaseInfo tile2dBaseInfo,
            DataType dataType,
            IntPtr pointerTileData,
            long sizeTileData);
    }
}
