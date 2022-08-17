#pragma once

namespace imgdoc2
{
    class ICreateOptions;

    class ClassFactory
    {
    public:
        static ICreateOptions* CreateCreateOptions();
    };
}