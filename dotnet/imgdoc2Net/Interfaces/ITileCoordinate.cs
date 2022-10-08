namespace ImgDoc2Net.Interfaces
{
    using System;
    using System.Collections.Generic;
    using System.Text;

    /// <summary>   
    /// This interface is used to represent the "tile-coordinate" of a tile. 
    /// </summary>
    public interface ITileCoordinate : IEquatable<ITileCoordinate>

    {
    /// <summary>
    /// Tries the get coordinate of the specified dimension.
    /// </summary>
    /// <param name="imageDimension">The image dimension.</param>
    /// <param name="coord">If successful, The coordinate for the specified dimension.</param>
    /// <returns>True if successful, false otherwise.</returns>
    bool TryGetCoordinate(Dimension imageDimension, out int coord);

    /// <summary>
    /// Enumerates the dimensions and the coordinates.
    /// </summary>
    /// <returns>Enumeration of the dimensions and the coordinates.</returns>
    IEnumerable<Tuple<Dimension, int>> EnumCoordinates();
    }
}
