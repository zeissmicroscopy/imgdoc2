#pragma once

#include <cstdint>

namespace imgdoc2
{
    struct TileBaseInfo
    {
        std::uint32_t pixelWidth;
        std::uint32_t pixelHeight;
        std::uint8_t pixelType;
    };
}
