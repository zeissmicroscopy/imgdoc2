namespace ImgDoc2Net.Interfaces
{
    /// <summary> 
    /// Values that represent the "imgdoc2-data types" of tiles.
    /// Keep this in sync with the respective native enum, and the conversion-method in imgdoc2API/utilities.cpp.
    /// </summary>
    public enum DataType
    {
        Zero = 0,
        UncompressedBitmap = 1,
    }
}
