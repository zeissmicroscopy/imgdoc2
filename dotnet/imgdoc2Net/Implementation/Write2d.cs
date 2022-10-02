

namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using System.Text;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;

    internal partial class Write2d : IWrite2d
    {
        private IntPtr writer2dObjectHandle;

        private Write2d()
        {
        }

        public Write2d(IntPtr handle)
        {
            this.writer2dObjectHandle = handle;
        }

        public long AddTile(ITileCoordinate tileCoordinate, in LogicalPosition logicalPosition, Tile2dBaseInfo tile2dBaseInfo,
            DataType dataType, IntPtr pointerTileData, long sizeTileData)
        {
            ImgDoc2ApiInterop.Instance.Writer2dAddTile(
                this.writer2dObjectHandle,
                tileCoordinate,
                in logicalPosition,
                tile2dBaseInfo,
                dataType,
                pointerTileData,
                sizeTileData);
            return 0;
        }
    }

    /// <content>
    /// This part contains the implementation of IDisposable. 
    /// </content>
    internal partial class Write2d
    {
        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            this.ReleaseUnmanagedResources();
            if (disposing)
            {
            }
        }

        private void ReleaseUnmanagedResources()
        {
            ImgDoc2ApiInterop.Instance.DestroyWriter2d(this.writer2dObjectHandle);
            this.writer2dObjectHandle = IntPtr.Zero;
        }
    }
}
