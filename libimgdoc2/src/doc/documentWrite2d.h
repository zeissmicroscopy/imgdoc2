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
        imgdoc2::TileDataStorageType storage_type,
        const imgdoc2::IDataObjBase* data) override;

    virtual void BeginTransaction() override;
    virtual void CommitTransaction() override;
    virtual void RollbackTransaction() override;

    virtual ~DocumentWrite2d() override = default;

private:
    imgdoc2::dbIndex AddTileInternal(
        const imgdoc2::ITileCoordinate* coord,
        const imgdoc2::LogicalPositionInfo* info,
        const imgdoc2::TileBaseInfo* tileInfo,
        imgdoc2::DataTypes datatype,
        imgdoc2::TileDataStorageType storage_type,
        const imgdoc2::IDataObjBase* data);
    
    void AddToSpatialIndex(imgdoc2::dbIndex index, const imgdoc2::LogicalPositionInfo& logical_position_info);

    imgdoc2::dbIndex AddTileData(const imgdoc2::TileBaseInfo* tile_info, imgdoc2::DataTypes datatype, imgdoc2::TileDataStorageType storage_type, const imgdoc2::IDataObjBase* data);
    imgdoc2::dbIndex AddBlobData(imgdoc2::TileDataStorageType storage_type, const imgdoc2::IDataObjBase* data);

    std::shared_ptr<IDbStatement> CreateInsertDataStatement(const imgdoc2::IDataObjBase* data);

public:
    // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
    DocumentWrite2d() = default;
    DocumentWrite2d(const DocumentWrite2d&) = delete;             // copy constructor
    DocumentWrite2d& operator=(const DocumentWrite2d&) = delete;  // copy assignment
    DocumentWrite2d(DocumentWrite2d&&) = delete;                  // move constructor
    DocumentWrite2d& operator=(DocumentWrite2d&&) = delete;       // move assignment
};
