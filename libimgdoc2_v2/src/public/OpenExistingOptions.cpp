#include <string>
#include <unordered_set>
#include <imgdoc2.h>

using namespace imgdoc2;

class OpenExistingOptions : public imgdoc2::IOpenExistingOptions
{
private:
    std::string     filename_;
    bool            read_only_;
public:
    OpenExistingOptions() : read_only_(false)
    {}

    virtual void SetFilename(const char* sz) override
    {
        this->filename_ = sz;
    }

    virtual void SetOpenReadonly(bool read_only) override
    {
        this->read_only_ = read_only;
    }

    virtual bool GetOpenReadonly() const override
    {
        return this->read_only_;
    }

    virtual const std::string& GetFilename() const override
    {
        return this->filename_;
    }
};

/*static*/IOpenExistingOptions* imgdoc2::ClassFactory::CreateOpenExistingOptions()
{
    return new OpenExistingOptions();
}