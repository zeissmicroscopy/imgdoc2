#pragma once

#include "TileBaseInfo.h"
#include "DataTypes.h"
#include "IDataObj.h"

namespace imgdoc2
{
    class IDocWrite2d
    {
    public:
        virtual ~IDocWrite2d() = default;

        virtual imgdoc2::dbIndex AddTile(
            const imgdoc2::ITileCoordinate* coord,
            const imgdoc2::LogicalPositionInfo* info,
            const imgdoc2::TileBaseInfo* tileInfo,
            imgdoc2::DataTypes datatype,
            const imgdoc2::IDataObjBase* data) = 0;

    };
}
