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

/*static*/imgdoc2::LogicalPositionInfo Utilities::ConvertLogicalPositionInfoInteropToImgdoc2(const LogicalPositionInfoInterop& logical_position_info_interop)
{
    LogicalPositionInfo logical_position_info
    {
        logical_position_info_interop.position_x,
        logical_position_info_interop.position_y,
        logical_position_info_interop.width,
        logical_position_info_interop.height,
        logical_position_info_interop.pyramid_level
    };

    return logical_position_info;
}