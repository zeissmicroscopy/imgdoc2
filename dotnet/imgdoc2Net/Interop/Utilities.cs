namespace ImgDoc2Net.Interop
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    internal class Utilities
    {
        /// <summary>
        /// Gets a value indicating if the current platform is Linunx.
        /// </summary>
        public static bool IsLinux()
        {
            // http://stackoverflow.com/a/5117005/358336
            int p = (int)Environment.OSVersion.Platform;
            return (p == 4) || (p == 6) || (p == 128);
        }
    }
}
