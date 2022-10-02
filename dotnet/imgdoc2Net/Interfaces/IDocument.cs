using System;
using System.Collections.Generic;
using System.Text;

namespace ImgDoc2Net.Interfaces
{
    public interface IDocument : IDisposable
    {
        IRead2d Get2dReader();

        IWrite2d Get2dWriter();
    }
}
