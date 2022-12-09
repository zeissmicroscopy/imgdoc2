namespace ImgDoc2Net
{
    using System;
    using ImgDoc2Net.Implementation;
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;

    public partial class Document : IDocument
    {
        private IntPtr documentHandle;

        private Document()
        {
        }

        private Document(CreateOptions createOptions)
        {
            this.documentHandle = ImgDoc2ApiInterop.Instance.CreateNewDocument(createOptions.Handle);
        }

        public IRead2d Get2dReader()
        {
            var readerHandle = ImgDoc2ApiInterop.Instance.DocumentGetReader2d(this.documentHandle);
            if (readerHandle != IntPtr.Zero)
            {
                return new Read2d(readerHandle);
            }

            return null;
        }

        public IWrite2d Get2dWriter()
        {
            var writerHandle = ImgDoc2ApiInterop.Instance.DocumentGetWriter2d(this.documentHandle);
            if (writerHandle != IntPtr.Zero)
            {
                return new Write2d(writerHandle);
            }

            return null;
        }
    }

    public partial class Document
    {
        public static Document CreateNew(CreateOptions createOptions)
        {
            return new Document(createOptions);
        }
    }

    /// <content>   This part contains the implementation of the IDisposable interface. </content>
    public partial class Document
    {
        private bool disposedValue;

        /// <summary>   
        /// Finalizes an instance of the <see cref="Document"/> class..
        /// </summary>
        ~Document()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            this.Dispose(disposing: false);
        }

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged
        /// resources.
        /// </summary>
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            this.Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposedValue)
            {
                if (disposing)
                {
                    // TODO: dispose managed state (managed objects)
                }

                ImgDoc2ApiInterop.Instance.DestroyDocument(this.documentHandle);
                
                // TODO: set large fields to null
                this.disposedValue = true;
            }
        }
    }
}
