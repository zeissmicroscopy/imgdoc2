#pragma once

#pragma pack(push, 4)
struct LogicalPositionInfoInterop
{
    double position_x;
    double position_y;
    double width;
    double height;
    int pyramid_level;
};
#pragma pack(pop)