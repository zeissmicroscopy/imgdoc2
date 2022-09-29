#pragma once

#include <imgdoc2.h>

#include "logicalpositioninfointerop.h"
#include "tilecoordinateinterop.h"

class Utilities
{
public:
    static imgdoc2::TileCoordinate ConvertToTileCoordinate(const TileCoordinateInterop* tile_coordinate_interop);
    static imgdoc2::LogicalPositionInfo ConvertLogicalPositionInfoInteropToImgdoc2(const LogicalPositionInfoInterop& logical_position_info_interop);
};
