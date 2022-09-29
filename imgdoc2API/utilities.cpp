#include "utilities.h"

using namespace imgdoc2;

/*static*/imgdoc2::TileCoordinate Utilities::ConvertToTileCoordinate(const TileCoordinateInterop* tile_coordinate_interop)
{
  TileCoordinate tile_coordinate;
  for (int i = 0; i < tile_coordinate_interop->number_of_elements; ++i)
  {
    tile_coordinate.Set(
      tile_coordinate_interop->values[i].dimension,
      tile_coordinate_interop->values[i].value);
  }

  return tile_coordinate;
}