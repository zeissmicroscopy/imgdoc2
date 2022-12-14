#pragma once
#include "LogicalPositionInfo.h"
#include "ITileCoordinate.h"
#include "LogicalPositionInfo.h"
#include "IDimCoordinateQueryClause.h"
#include "ITIleInfoQueryClause.h"
#include "IBlobOutput.h"

namespace imgdoc2
{
    /// This interface is providing read-only access to a 2D-document.
    class IDocRead2d
    {
    public:
        virtual ~IDocRead2d() = default;

        virtual void ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info) = 0;

        /// Query the tiles table. The 
       /// \param clause        The query clause (dealing with dimension indexes).
       /// \param tileInfoQuery The query clause (dealing with other "per tile data").
       /// \param func          A functor which we will called, passing in the index of tiles matching the query. If the functor returns false, the enumeration is canceled, and no
       ///                      more calls to the functor will occur any more.
        virtual void Query(const imgdoc2::IDimCoordinateQueryClause* clause, const imgdoc2::ITileInfoQueryClause* tileInfoQuery, const std::function<bool(imgdoc2::dbIndex)>& func) = 0;

        virtual void GetTilesIntersectingRect(const imgdoc2::RectangleD& rect, const imgdoc2::IDimCoordinateQueryClause* coordinate_clause, const imgdoc2::ITileInfoQueryClause* tileinfo_clause, std::function<bool(imgdoc2::dbIndex)> func) = 0;

        virtual void ReadTileData(imgdoc2::dbIndex idx, imgdoc2::IBlobOutput* data) = 0;
    public:
        void GetTilesIntersectingRect(const imgdoc2::RectangleD& rect, const std::function<bool(imgdoc2::dbIndex)>& func)
        {
            this->GetTilesIntersectingRect(rect, nullptr, nullptr, func);
        }
    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDocRead2d() = default;
        IDocRead2d(const IDocRead2d&) = delete;             // copy constructor
        IDocRead2d& operator=(const IDocRead2d&) = delete;  // copy assignment
        IDocRead2d(IDocRead2d&&) = delete;                  // move constructor
        IDocRead2d& operator=(IDocRead2d&&) = delete;       // move assignment
    };
}
