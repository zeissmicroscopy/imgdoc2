#pragma once

namespace imgdoc2
{
    class IDbIndexManagement
    {
    public:
        virtual void DropIndexOnCoordinate(TileDim dim) = 0;
        virtual void CreateIndexOnCoordinate(TileDim dim) = 0;

        virtual void DropSpatialIndex() = 0;
        virtual void CreateSpatialIndex() = 0;

        virtual ~IDbIndexManagement() = default;
    };
}