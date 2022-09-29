
namespace ImgDoc2Net
{
    using System;

    /// <summary>   This structure is representing a 'dimension' in imgdoc2. </summary>
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
