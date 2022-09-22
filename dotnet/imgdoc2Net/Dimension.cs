using System;
using System.Collections.Generic;
using System.Text;

namespace ImgDoc2Net
{
    public struct Dimension
    {
        public Dimension(char c)
        {
            Dimension.ThrowIfInvalidChar(c);
            this.Id = c;
        }

        public char Id;

        private static void ThrowIfInvalidChar(char c)
        {
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')))
            {
                throw new ArgumentException("Dimension must be a-zA-Z.");
            }
        }
    }
}
