namespace ImgDoc2Net.Implementation
{
    using System;
    using System.Collections.Generic;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;

    internal partial class Read2d : IRead2d
    {
        private IntPtr reader2dObjectHandle;

        /// <summary>   Initializes a new instance of the <see cref="Read2d"/> class. </summary>
        /// <param name="handle" type="IntPtr"> The handle. </param>
        public Read2d(IntPtr handle)
        {
            this.reader2dObjectHandle = handle;
        }

        private Read2d()
        {
        }

        public List<long> Query(IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions)
        {
            // TODO(Jbl): error-handling
            var queryResult = ImgDoc2ApiInterop.Instance.Reader2dQuery(
                this.reader2dObjectHandle, 
                queryClause, 
                tileInfoQueryClause, 
                queryOptions != null ? queryOptions.MaxNumbersOfResults : QueryOptions.DefaultMaxNumberOfResults);
            if (queryOptions != null)
            {
                queryOptions.ResultWasComplete = queryResult.ResultComplete;
            }

            return queryResult.Keys;
        }

        public byte[] ReadTileData(long key)
        {
            // TODO(Jbl): error-handling
            return ImgDoc2ApiInterop.Instance.Reader2dReadTileData(this.reader2dObjectHandle, key);
        }

        public List<long> QueryTilesIntersectingRect(Rectangle rectangle, IDimensionQueryClause queryClause, ITileInfoQueryClause tileInfoQueryClause, QueryOptions queryOptions)
        {
            // TODO(Jbl): error-handling
            var queryResult = ImgDoc2ApiInterop.Instance.Reader2dGetTilesIntersectingRect(
                this.reader2dObjectHandle, 
                rectangle, 
                queryClause, 
                tileInfoQueryClause,
                queryOptions != null ? queryOptions.MaxNumbersOfResults : QueryOptions.DefaultMaxNumberOfResults);
            if (queryOptions != null)
            {
                queryOptions.ResultWasComplete = queryResult.ResultComplete;
            }

            return queryResult.Keys;
        }
    }

    /// <content>
    /// This part contains the implementation of IDisposable. 
    /// </content>
    internal partial class Read2d
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
            ImgDoc2ApiInterop.Instance.DestroyReader2d(this.reader2dObjectHandle);
            this.reader2dObjectHandle = IntPtr.Zero;
        }
    }
}