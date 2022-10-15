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
    const LogicalPositionInfo logical_position_info
    {
        logical_position_info_interop.position_x,
        logical_position_info_interop.position_y,
        logical_position_info_interop.width,
        logical_position_info_interop.height,
        logical_position_info_interop.pyramid_level
    };

    return logical_position_info;
}

/*static*/imgdoc2::CDimCoordinateQueryClause Utilities::ConvertDimensionQueryRangeClauseInteropToImgdoc2(const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop)
{
    CDimCoordinateQueryClause query_clause;
    for (int i = 0; i < dim_coordinate_query_clause_interop->element_count; ++i)
    {
        query_clause.AddRangeClause(
            dim_coordinate_query_clause_interop->query_range_clause[i].dimension,
            IDimCoordinateQueryClause::RangeClause{ dim_coordinate_query_clause_interop->query_range_clause[i].start, dim_coordinate_query_clause_interop->query_range_clause[i].end });
    }

    return query_clause;
}

/*static*/imgdoc2::CTileInfoQueryClause Utilities::ConvertTileInfoQueryClauseInteropToImgdoc2(const TileInfoQueryClauseInterop* tile_info_query_clause_interop)
{
    CTileInfoQueryClause tile_info_query_clause;
    for (int i = 0; i < tile_info_query_clause_interop->pyramid_level_condition_count; ++i)
    {
        LogicalOperator logical_operator = ConvertToLogicalOperatorEnum(tile_info_query_clause_interop->pyramid_level_condition[i].logical_operator);
        ComparisonOperation  comparison_operation = ConvertToComparisonOperationEnum(tile_info_query_clause_interop->pyramid_level_condition[i].comparison_operator);
        int value = tile_info_query_clause_interop->pyramid_level_condition[i].value;
        tile_info_query_clause.AddPyramidLevelCondition(logical_operator, comparison_operation, value);
    }

    return tile_info_query_clause;
}

/*static*/imgdoc2::TileBaseInfo Utilities::ConvertTileBaseInfoInteropToImgdoc2(const TileBaseInfoInterop& tile_base_info_interop)
{
    TileBaseInfo tile_base_info;
    tile_base_info.pixelWidth = tile_base_info_interop.pixelWidth;
    tile_base_info.pixelHeight = tile_base_info_interop.pixelHeight;
    tile_base_info.pixelType = tile_base_info_interop.pixelType;
    return tile_base_info;
}

/*static*/imgdoc2::DataTypes Utilities::ConvertDatatypeEnumInterop(std::uint8_t data_type_interop)
{
    // TODO: add error handling
    return static_cast<imgdoc2::DataTypes>(data_type_interop);
}

/*static*/imgdoc2::RectangleD Utilities::ConvertRectangleDoubleInterop(const RectangleDoubleInterop& rectangle_interop)
{
    return RectangleD(rectangle_interop.x, rectangle_interop.y, rectangle_interop.width, rectangle_interop.height);
}

imgdoc2::LogicalOperator Utilities::ConvertToLogicalOperatorEnum(std::uint8_t value)
{
    if (value > static_cast<underlying_type_t<LogicalOperator>>(LogicalOperator::MaxValue))
    {
        throw invalid_argument("invalid value for enum \"LogicalOperator\".");
    }

    return static_cast<LogicalOperator>(value);
}

imgdoc2::ComparisonOperation Utilities::ConvertToComparisonOperationEnum(std::uint8_t value)
{
    if (value > static_cast<underlying_type_t<ComparisonOperation>>(ComparisonOperation::MaxValue))
    {
        throw invalid_argument("invalid value for enum \"ComparisonOperation\".");
    }

    return static_cast<ComparisonOperation>(value);
}