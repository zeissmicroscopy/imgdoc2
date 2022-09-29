#pragma once

#include <imgdoc2.h>
#include "tilecoordinateinterop.h"

class Utilities
{
public:
    static imgdoc2::TileCoordinate ConvertToTileCoordinate(const TileCoordinateInterop* tile_coordinate_interop);
};
