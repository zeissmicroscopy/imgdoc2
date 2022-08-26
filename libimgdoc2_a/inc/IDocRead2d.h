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

    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocRead2d() = default;
        IDocRead2d(const IDocRead2d&) = delete;             // copy constructor
        IDocRead2d& operator=(const IDocRead2d&) = delete;  // copy assignment
        IDocRead2d(IDocRead2d&&) = delete;                  // move constructor
        IDocRead2d& operator=(IDocRead2d&&) = delete;       // move assignment
    };
}
