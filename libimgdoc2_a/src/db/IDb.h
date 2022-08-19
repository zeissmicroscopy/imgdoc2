#pragma once

#include <imgdoc.h>

class IDb
{
public:
    virtual std::shared_ptr<imgdoc::IDbWrite> GetWriter() = 0;
    virtual std::shared_ptr<imgdoc::IDbRead> GetReader() = 0;
};