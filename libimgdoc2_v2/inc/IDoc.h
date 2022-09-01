#pragma once

namespace imgdoc2
{
    class IDocWrite2d;
    class IDocRead2d;

    class IDoc
    {
    public:
        /// Try to get a "write object" for a 2D-document. This method may return an empty shared_ptr
   /// if such an object cannot be constructed.
   /// \returns The writer-object (for 2D-document).
        virtual std::shared_ptr<imgdoc2::IDocWrite2d> GetWriter2d() = 0;

        /// Try to get a "read object" for a 2D-document. This method may return an empty shared_ptr
        /// if such an object cannot be constructed.
        /// \returns The read-object (for 2D-document).
        virtual std::shared_ptr<imgdoc2::IDocRead2d> GetReader2d() = 0;

        virtual ~IDoc() = default;
    };
}