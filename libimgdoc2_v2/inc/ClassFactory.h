#pragma once

#include <memory>

namespace imgdoc2
{
    class ICreateOptions;
    class IDoc;

    /// Class factory creating objects implemented in the imgdoc2-library.
    class ClassFactory
    {
    public:

        /// Creates an options-object for creating a new imgdoc2-document.
        ///
        /// \returns    Pointer to a newly create options-object.
        static imgdoc2::ICreateOptions* CreateCreateOptions();

        /// Creates a new imgdoc2-document. A new database is constructed.
        ///
        /// \param [in] create_options  The options controlling the operation.
        ///
        /// \returns    The newly created imgdoc2-document.
        static std::shared_ptr<imgdoc2::IDoc> CreateNew(imgdoc2::ICreateOptions* create_options);
    };
}
