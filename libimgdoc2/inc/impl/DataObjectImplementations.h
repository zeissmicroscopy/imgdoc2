#pragma once

#include <cstring>
#include "../IDataObj.h"

namespace imgdoc2
{
    class DataObjectOnHeap : public imgdoc2::IDataObjBase
    {
    private:
        std::uint8_t* buffer_{ nullptr };
        size_t buffer_size_{ 0 };
    public:
        DataObjectOnHeap(size_t size)
        {
            this->buffer_ = static_cast<std::uint8_t*>(malloc(size));
            this->buffer_size_ = size;
        }

        void GetData(const void** p, size_t* s) const override
        {
            if (p != nullptr)
            {
                *p = this->buffer_;
            }

            if (s != nullptr)
            {
                *s = this->buffer_size_;
            }
        }

        ~DataObjectOnHeap() override
        {
            free(this->buffer_);
        }

    public:
        const std::uint8_t* GetDataC() const { return this->buffer_; }

        std::uint8_t* GetData() { return this->buffer_; }

        size_t GetSizeOfData() const { return this->buffer_size_; }

    public:
        // no copy and no move 
        DataObjectOnHeap() = delete;
        DataObjectOnHeap(const DataObjectOnHeap&) = delete;             // copy constructor
        DataObjectOnHeap& operator=(const DataObjectOnHeap&) = delete;  // copy assignment
        DataObjectOnHeap(DataObjectOnHeap&&) = delete;                  // move constructor
        DataObjectOnHeap& operator=(DataObjectOnHeap&&) = delete;       // move assignment
    };
}