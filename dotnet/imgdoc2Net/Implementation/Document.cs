namespace ImgDoc2Net
{
    using ImgDoc2Net.Interfaces;
    using ImgDoc2Net.Interop;
    using System;
    using System.Collections.Generic;
    using System.Text;

    public partial class Document : IDocument
    {
        private IntPtr documentHandle;

        private Document(CreateOptions createOptions)
        {
            this.documentHandle = ImgDoc2ApiInterop.Instance.CreateNewDocument(createOptions.Handle);
        }

        public IRead2d Get2dReader()
        {
            throw new NotImplementedException();
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

        protected virtual void Dispose(bool disposing)
        {
            if (!this.disposedValue)
            {
                if (disposing)
                {
                    // TODO: dispose managed state (managed objects)
                }

                // TODO: free unmanaged resources (unmanaged objects) and override finalizer
                // TODO: set large fields to null
                this.disposedValue = true;
            }
        }

        // TODO: override finalizer only if 'Dispose(bool disposing)' has code to free unmanaged resources
        ~Document()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            this.Dispose(disposing: false);
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            this.Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
    }
}
