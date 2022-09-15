#include <gtest/gtest.h>
#include "../libimgdoc2_v2/inc/imgdoc2.h"
#include "../libimgdoc2_v2/src/db/utilities.h"

using namespace std;
using namespace imgdoc2;

class BlobDataObj : public IDataObjBase
{
private:
    size_t  size_;
    void* data_;
public:
    BlobDataObj() = delete;
    BlobDataObj(size_t size) : size_(size)
    {
        this->data_ = malloc(size);
    }

    ~BlobDataObj() override
    {
        free(this->data_);
    }

    void GetData(const void** p, size_t* s) const override
    {
        if (p != nullptr)
        {
            *p = this->data_;
        }

        if (s != nullptr)
        {
            *s = this->size_;
        }
    }

    size_t GetSize() const { return this->size_; }
    void* GetDataPointer() { return this->data_; }
};

class BlobOutput : public IBlobOutput
{
private:
    size_t size_{ 0 };
    void* data_{ nullptr };
public:
    ~BlobOutput() override
    {
        free(this->data_);
    }

    bool Reserve(size_t s) override
    {
        // TODO: must only be called once
        this->size_ = s;
        this->data_ = malloc(s);
        return true;
    }

    bool SetData(size_t offset, size_t size, const void* data) override
    {
        memcpy(static_cast<char*>(this->data_) + offset, data, size);
        return true;
    }

    size_t GetSize() const { return this->size_; }
    void* GetDataPointer() { return this->data_; }
};

TEST(BlobData, AddTileWithBlobDataAndRetrieveItCheckForCorrectness)
{
    auto create_options = ClassFactory::CreateCreateOptionsUp();
    create_options->SetFilename(":memory:");
    //create_options->SetFilename("d:\\test.db");
    create_options->AddDimension('M');
    create_options->SetCreateBlobTable(true);

    auto doc = ClassFactory::CreateNew(create_options.get());
    auto writer = doc->GetWriter2d();

    LogicalPositionInfo position_info;
    TileBaseInfo tileInfo;
    TileCoordinate tc({ { 'M',0} });
    position_info.posX = 0;
    position_info.posY = 0;
    position_info.width = 10;
    position_info.height = 10;
    position_info.pyrLvl = 0;
    tileInfo.pixelWidth = 10;
    tileInfo.pixelHeight = 10;
    tileInfo.pixelType = 0;
    BlobDataObj blobData{ 100 };
    for (size_t i = 0; i < blobData.GetSize(); ++i)
    {
        uint8_t* ptrData = static_cast<uint8_t*>(blobData.GetDataPointer()) + i;
        *ptrData = static_cast<uint8_t>(i);
    }

    writer->AddTile(
        &tc,
        &position_info,
        &tileInfo,
        DataTypes::UNCOMPRESSED_BITMAP,
        TileDataStorageType::BlobInDatabase,
        &blobData);

    auto reader = doc->GetReader2d();
    CDimCoordinateQueryClause coordinate_query_clause;
    coordinate_query_clause.AddRangeClause('M', IDimCoordinateQueryClause::RangeClause{ 0, 0 });
    vector<dbIndex> indices;
    reader->Query(&coordinate_query_clause, nullptr, [&](dbIndex index)->bool {indices.push_back(index); return true; });
    ASSERT_EQ(indices.size(), 1);

    BlobOutput blob;
    reader->ReadTileData(indices[0], &blob);
    ASSERT_EQ(blob.GetSize(), 100);

    for (size_t i = 0; i < blob.GetSize(); ++i)
    {
        uint8_t* ptrData = static_cast<uint8_t*>(blob.GetDataPointer()) + i;
        ASSERT_EQ(*ptrData, static_cast<uint8_t>(i));
    }
}