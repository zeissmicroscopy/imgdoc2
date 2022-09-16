#pragma once

#include <cstdint>
#include <memory>
#include <exception>
#include "../IBlobOutput.h"

namespace imgdoc2
{
    class BlobOutputOnHeap : public imgdoc2::IBlobOutput
    {
    private:
        std::uint8_t* buffer_{ nullptr };
        size_t buffer_size_{ 0 };
        bool is_reserved_{ false };
    public:
        ~BlobOutputOnHeap() override
        {
            free(this->buffer_);
        }

        bool Reserve(size_t s) override
        {
            this->buffer_ = static_cast<uint8_t*>(malloc(s));
            this->buffer_size_ = s;
            this->is_reserved_ = true;
            return true;
        }

        bool SetData(size_t offset, size_t size, const void* data)
        {
            if (!this->is_reserved_)
            {
                throw std::logic_error("'Reserve' was not called before.");
            }

            if (offset + size > this->buffer_size_)
            {
                throw std::invalid_argument("out-of-bounds");
            }

            memcpy(this->buffer_ + offset, data, size);
            return true;
        }

    public:
        bool GetHasData() const { return this->is_reserved_; }

        const std::uint8_t* GetDataC() const { return this->buffer_; }

        std::uint8_t* GetData() { return this->buffer_; }

        size_t GetSizeOfData() const { return this->buffer_size_; }

    public:
        // no copy and no move 
        BlobOutputOnHeap() = default;
        BlobOutputOnHeap(const BlobOutputOnHeap&) = delete;             // copy constructor
        BlobOutputOnHeap& operator=(const BlobOutputOnHeap&) = delete;  // copy assignment
        BlobOutputOnHeap(BlobOutputOnHeap&&) = delete;                  // move constructor
        BlobOutputOnHeap& operator=(BlobOutputOnHeap&&) = delete;       // move assignment
    };
}