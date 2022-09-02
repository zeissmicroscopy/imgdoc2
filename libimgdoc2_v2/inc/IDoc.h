#pragma once

namespace imgdoc2
{
    class IDocWrite2d;
    class IDocRead2d;

    enum class DocumentType
    {
        Invalid,
        Image2d,
        Image3d
    };

    /// This interface is representing a 'document'. The discovery phase of the document has been completed successfully.
    /// Depending on the type of the document, objects for interacting with it can be created.
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
    public:
        // no copy and no move (-> https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c21-if-you-define-or-delete-any-copy-move-or-destructor-function-define-or-delete-them-all )
        IDoc() = default;
        IDoc(const IDocRead2d&) = delete;             // copy constructor
        IDoc& operator=(const IDocRead2d&) = delete;  // copy assignment
        IDoc(IDoc&&) = delete;                        // move constructor
        IDoc& operator=(IDoc&&) = delete;             // move assignment
    };
}