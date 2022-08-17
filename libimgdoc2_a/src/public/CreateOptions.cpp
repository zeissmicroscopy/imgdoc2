#include <string>
#include <unordered_set>
#include <imgdoc2.h>


using namespace imgdoc2;

class CreateOptions: public imgdoc2::ICreateOptions
{
private:
    std::string     filename_;
    std::unordered_set<Dimension> dimensions_;
public:
    void SetFilename(const char* sz) override
    {
        this->filename_ = sz;
    }

    const std::string& GetFilename() const override
    {
        return this->filename_;
    }

    void AddDimension(Dimension dim) override
    {
        this->dimensions_.emplace(dim);
    }

    const std::unordered_set<Dimension>& GetDimensions() override
    {
        return this->dimensions_;
    }
};

/*static*/ICreateOptions* imgdoc2::ClassFactory::CreateCreateOptions()
{
    return new CreateOptions();
}
