#pragma once
#include "LogicalPositionInfo.h"
#include "ITileCoordinate.h"
#include "LogicalPositionInfo.h"

namespace imgdoc2
{
    class IDocRead2d
    {
    public:
        virtual ~IDocRead2d() = default;

        virtual void ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info) = 0;
    };
}