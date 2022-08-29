#include <imgdoc2.h>
#include <memory>
#include "document.h"
#include "ITileCoordinate.h"

class DocumentRead2d : public imgdoc2::IDocRead2d
{
private:
    std::shared_ptr < Document> document_;
public:
    DocumentRead2d(std::shared_ptr < Document> document) : document_(std::move(document))
    {}

    virtual void ReadTileInfo(imgdoc2::dbIndex idx, imgdoc2::ITileCoordinateMutate* coord, imgdoc2::LogicalPositionInfo* info) override;
    virtual void Query(const imgdoc2::IDimCoordinateQueryClause* clause, const imgdoc2::ITileInfoQueryClause* tileInfoQuery, const std::function<bool(imgdoc2::dbIndex)>& func) override;

private:
    std::shared_ptr<IDbStatement> GetReadTileInfo_Statement(bool include_tile_coordinates, bool include_logical_position_info);
    std::shared_ptr<IDbStatement> CreateQueryStatement(const imgdoc2::IDimCoordinateQueryClause* clause, const imgdoc2::ITileInfoQueryClause* tileInfoQuery);
};
