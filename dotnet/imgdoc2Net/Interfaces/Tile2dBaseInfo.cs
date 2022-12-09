namespace ImgDoc2Net.Interfaces
{
    public class Tile2dBaseInfo
    {
        public Tile2dBaseInfo(int width, int height, PixelType pixelType)
        {
            this.PixelWidth = width;
            this.PixelHeight = height;
            this.PixelType = pixelType;
        }

        public int PixelWidth { get; set; }

        public int PixelHeight { get; set; }

        public PixelType PixelType { get; set; }
    }
}
