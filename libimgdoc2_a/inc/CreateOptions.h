#pragma once

#include <string>
#include <unordered_set>

namespace imgdoc2
{
    typedef char Dimension;

    class ICreateOptions
    {
    public:

        virtual void SetFilename(const char* sz) = 0;

        virtual const std::string& GetFilename() const = 0;

        virtual void AddDimension(Dimension dim) = 0;

        template<class ForwardIterator>
            void AddDimensions(ForwardIterator start, ForwardIterator end)
        {
            while (start != end)
            {
                this->AddDimension(*start++);
            }
        }

        virtual const std::unordered_set<Dimension>& GetDimensions() = 0;

        virtual ~ICreateOptions() = default;
    };
}