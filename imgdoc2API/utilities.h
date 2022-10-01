#pragma once

#include <imgdoc2.h>

#include "logicalpositioninfointerop.h"
#include "tilecoordinateinterop.h"
#include "dimcoordinatequeryclauseinterop.h"
#include "tilebaseinfointerop.h"

class Utilities
{
public:
    static imgdoc2::TileCoordinate ConvertToTileCoordinate(const TileCoordinateInterop* tile_coordinate_interop);
    static imgdoc2::LogicalPositionInfo ConvertLogicalPositionInfoInteropToImgdoc2(const LogicalPositionInfoInterop& logical_position_info_interop);
    static imgdoc2::CDimCoordinateQueryClause ConvertDimensionQueryRangeClauseInteropToImgdoc2(const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop);
    static imgdoc2::TileBaseInfo ConvertTileBaseInfoInteropToImgdoc2(const TileBaseInfoInterop& tile_base_info_interop);
};
