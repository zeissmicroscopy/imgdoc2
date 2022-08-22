#include <string>
#include <unordered_set>
#include <imgdoc2.h>


using namespace imgdoc2;

class CreateOptions: public imgdoc2::ICreateOptions
{
private:
    std::string     filename_;
    std::unordered_set<Dimension> dimensions_;
    std::unordered_set<Dimension> dimensionsToIndex_;
    bool            use_spatial_index_;
public:
    CreateOptions() : use_spatial_index_(false)
    {}

    void SetFilename(const char* sz) override
    {
        this->filename_ = sz;
    }

    const std::string& GetFilename() const override
    {
        return this->filename_;
    }

    void SetUseSpatialIndex(bool use_spatial_index) override
    {
        this->use_spatial_index_ = use_spatial_index;
    }

    bool GetUseSpatialIndex() const override
    {
        return this->use_spatial_index_;
    }

    void AddDimension(Dimension dim) override
    {
        this->dimensions_.emplace(dim);
    }

    void AddIndexForDimension(Dimension dim) override
    {
        this->dimensionsToIndex_.emplace(dim);
    }

    const std::unordered_set<Dimension>& GetDimensions() const override
    {
        return this->dimensions_;
    }
};

/*static*/ICreateOptions* imgdoc2::ClassFactory::CreateCreateOptions()
{
    return new CreateOptions();
}
