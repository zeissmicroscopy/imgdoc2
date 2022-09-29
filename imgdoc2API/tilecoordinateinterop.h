#pragma once

#pragma pack(push, 4)
struct DimensionAndValueInterop
{
    imgdoc2::Dimension dimension;
    int value;
};
#pragma pack(pop)

#pragma pack(push, 4)
struct TileCoordinateInterop
{
    int number_of_elements;
    DimensionAndValueInterop values[];
};
#pragma pack(pop)