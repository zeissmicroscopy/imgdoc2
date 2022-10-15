#pragma once

#include <cstdint>

namespace imgdoc2
{
    struct TileBaseInfo
    {
        std::uint32_t pixelWidth{0};
        std::uint32_t pixelHeight{0};
        std::uint8_t pixelType;
    };
}
