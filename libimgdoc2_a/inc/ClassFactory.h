#pragma once

#include <memory>

namespace imgdoc2
{
    class ICreateOptions;
    class IDoc;

    class ClassFactory
    {
    public:
        static imgdoc2::ICreateOptions* CreateCreateOptions();

        static std::shared_ptr<imgdoc2::IDoc> CreateNew(imgdoc2::ICreateOptions*);
    };
}
