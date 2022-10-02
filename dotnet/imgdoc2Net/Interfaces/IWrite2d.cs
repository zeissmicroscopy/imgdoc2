

namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

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

    public static class Write2dExtensions
    {
        public static long AddTile(
            this IWrite2d write2,
            ITileCoordinate tileCoordinate,
            in LogicalPosition logicalPosition,
            Tile2dBaseInfo tile2dBaseInfo,
            DataType dataType,
            byte[] data)
        {
            unsafe
            {
                fixed (byte* pointerToData = &data[0])
                {
                    return write2.AddTile(tileCoordinate, in logicalPosition, tile2dBaseInfo, dataType, new IntPtr(pointerToData), data.Length);
                }
            }
        }
    }

}
