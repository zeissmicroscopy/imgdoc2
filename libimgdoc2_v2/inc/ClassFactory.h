#pragma once

#include <memory>
#include "ICreateOptions.h"
#include "IOpenExistingOptions.h"

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

        static imgdoc2::IOpenExistingOptions* CreateOpenExistingOptions();

        /// Creates a new imgdoc2-document. A new database is constructed.
        ///
        /// \param [in] create_options  The options controlling the operation.
        ///
        /// \returns    The newly created imgdoc2-document.
        static std::shared_ptr<imgdoc2::IDoc> CreateNew(imgdoc2::ICreateOptions* create_options);

        static std::shared_ptr<imgdoc2::IDoc> OpenExisting(imgdoc2::IOpenExistingOptions* open_existing_options);
    };
}
