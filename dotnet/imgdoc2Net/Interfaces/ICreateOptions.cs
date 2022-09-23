namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    public interface ICreateOptions : IDisposable
    {
        string Filename { get; }

        bool UseSpatialIndex { get; }

        void AddDimension(Dimension dimension);

        void AddIndexedDimension(Dimension dimension);
    }
}
