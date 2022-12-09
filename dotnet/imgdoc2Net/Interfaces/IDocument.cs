namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    public interface IDocument : IDisposable
    {
        IRead2d Get2dReader();

        IWrite2d Get2dWriter();
    }
}
