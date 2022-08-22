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

        virtual void SetUseSpatialIndex(bool use_spatial_index)=0;

        virtual bool GetUseSpatialIndex() const=0;

        virtual void AddIndexForDimension(Dimension dim) = 0;

        template<class ForwardIterator>
            void AddDimensions(ForwardIterator start, ForwardIterator end)
        {
            while (start != end)
            {
                this->AddDimension(*start++);
            }
        }

        virtual const std::unordered_set<Dimension>& GetDimensions() const = 0;

        virtual ~ICreateOptions() = default;
    };
}