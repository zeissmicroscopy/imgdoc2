#include <imgdoc2.h>
#include "document.h"
#include "ITileCoordinate.h"

class DocumentWrite2d : public imgdoc2::IDocWrite2d
{
private:
    std::shared_ptr < Document> document_;
public:
    DocumentWrite2d(std::shared_ptr < Document> document) : document_(std::move(document))
    {}

    virtual imgdoc2::dbIndex AddTile(
        const imgdoc2::ITileCoordinate* coord, 
        const imgdoc2::LogicalPositionInfo* info, 
        const imgdoc2::TileBaseInfo* tileInfo, 
        imgdoc2::DataTypes datatype, 
        const imgdoc2::IDataObjBase* data) override;

    virtual ~DocumentWrite2d() = default;

private:
    imgdoc2::dbIndex AddTileData(std::uint32_t width, std::uint32_t height, std::uint8_t pixeltype, imgdoc2::DataTypes datatype);
};
