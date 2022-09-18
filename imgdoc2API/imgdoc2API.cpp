#include <memory>
#include <imgdoc2.h>
#include "imgdoc2API.h"

using namespace imgdoc2;

template <typename t>
struct SharedPtrWrapper
{
    SharedPtrWrapper(std::shared_ptr<t> shared_ptr) : shared_ptr_(std::move(shared_ptr)) {}
    std::shared_ptr<t> shared_ptr_;
};

HandleCreateOptions CreateCreateOptions()
{
    return reinterpret_cast<HandleCreateOptions>(ClassFactory::CreateCreateOptionsPtr());
}

void DestroyCreateOptions(HandleCreateOptions handle)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
}

HandleOpenExistingOptions CreateOpenExistingOptions()
{
    return reinterpret_cast<HandleOpenExistingOptions>(ClassFactory::CreateOpenExistingOptions());
}

void DestroyOpenExistingOptions(HandleOpenExistingOptions handle)
{
    const auto object = reinterpret_cast<IOpenExistingOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
}

ImgDoc2ErrorCode CreateNewDocument(HandleCreateOptions create_options, HandleDoc* document, ImgDoc2ErrorInformation* error_information)
{
    if (document == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto imgdoc2 = ClassFactory::CreateNew(reinterpret_cast<ICreateOptions*>(create_options));  // NOLINT(performance-no-int-to-ptr)
    auto shared_imgdoc_wrappping_object = new SharedPtrWrapper<IDoc>{ imgdoc2 };
    *document = reinterpret_cast<HandleDoc>(shared_imgdoc_wrappping_object);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode OpenExistingDocument(HandleOpenExistingOptions open_existing_options, HandleDoc* document, ImgDoc2ErrorInformation* error_information)
{
    if (document == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto imgdoc2 = ClassFactory::OpenExisting(reinterpret_cast<IOpenExistingOptions*>(open_existing_options));  // NOLINT(performance-no-int-to-ptr)
    auto shared_imgdoc_wrappping_object = new SharedPtrWrapper<IDoc>{ imgdoc2 };
    *document = reinterpret_cast<HandleDoc>(shared_imgdoc_wrappping_object);
    return ImgDoc2_ErrorCode_OK;
}

void DestroyDocument(HandleDoc handle)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
}

ImgDoc2ErrorCode IDoc_GetReader2d(HandleDoc handle_document, HandleDocRead2D* reader, ImgDoc2ErrorInformation* error_information)
{
    if (reader == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto spReader2d = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document)->shared_ptr_->GetReader2d();   // NOLINT(performance-no-int-to-ptr)
    if (spReader2d)
    {
        auto shared_reader2d_wrappping_object = new SharedPtrWrapper<IDocRead2d>{ spReader2d };
        *reader = reinterpret_cast<HandleDocRead2D>(shared_reader2d_wrappping_object);
    }
    else
    {
        *reader = kInvalidOjectHandle;
    }

    return ImgDoc2_ErrorCode_OK;
}

void DestroyReader2d(HandleDocRead2D handle)
{
    const auto object = reinterpret_cast<SharedPtrWrapper<IDocRead2d>*>(handle);  // NOLINT(performance-no-int-to-ptr)
    delete object;
}

ImgDoc2ErrorCode CreateOptions_SetFilename(HandleCreateOptions handle, const char* filename_utf8)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->SetFilename(filename_utf8);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddDimension(HandleCreateOptions handle, char dimension)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->AddDimension(dimension);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_SetUseSpatialIndex(HandleCreateOptions handle, bool use_spatial_index)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->SetUseSpatialIndex(use_spatial_index);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddIndexForDimension(HandleCreateOptions handle, char dimension)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->AddIndexForDimension(dimension);
    return ImgDoc2_ErrorCode_OK;
}