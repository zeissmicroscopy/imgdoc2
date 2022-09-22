using System;
using System.Collections.Generic;
using System.Text;

namespace ImgDoc2Net.Interop
{
    [Serializable]
    public class ImgDoc2Exception : Exception
    {
        private int imgdoc2ErrorCode;

        public ImgDoc2Exception() { }

        public ImgDoc2Exception(int errorCode,string text)
            : base(text)
        {
            this.imgdoc2ErrorCode = errorCode;
        }

        public int ImgDoc2ErrorCode
        {
            get { return this.imgdoc2ErrorCode; }
        }
    }
}
