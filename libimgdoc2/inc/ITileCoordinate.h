#pragma once

#include <functional>
#include <vector>
#include "types.h"

namespace imgdoc2
{
    class ITileCoordinate
    {
    public:
        virtual bool TryGetCoordinate(imgdoc2::Dimension dim, int* coordVal) const = 0;
        virtual void EnumCoordinates(const std::function<bool(imgdoc2::Dimension, int)>& f) const = 0;
        virtual ~ITileCoordinate() = default;

        inline void EnumDimensions(std::function<bool(imgdoc2::Dimension)> f) const
        {
            this->EnumCoordinates([&](imgdoc2::Dimension d, int v)->bool {return f(d); });
        }

        inline std::vector<imgdoc2::Dimension> GetDimensions() const
        {
            std::vector<imgdoc2::Dimension> vec;
            this->EnumDimensions([&vec](imgdoc2::Dimension d)->bool {vec.push_back(d); return true; });
            return vec;
        }

        inline static bool IsValidDimension(imgdoc2::Dimension d)
        {
            return imgdoc2::IsDimensionValid(d);
        }
    };

    class ITileCoordinateMutate
    {
    public:
        virtual void Clear() = 0;
        virtual void Set(imgdoc2::Dimension d, int value) = 0;

        virtual ~ITileCoordinateMutate() = default;
    };
}
