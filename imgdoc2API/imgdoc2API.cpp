#include <memory>
#include <algorithm>
#include <imgdoc2.h>
#include "imgdoc2API.h"

using namespace imgdoc2;
using namespace std;

template <typename t>
struct SharedPtrWrapper
{
    SharedPtrWrapper(std::shared_ptr<t> shared_ptr) : shared_ptr_(std::move(shared_ptr)) {}
    std::shared_ptr<t> shared_ptr_;
};

static void FillOutErrorInformation(const exception& exception, ImgDoc2ErrorInformation* error_information)
{
    if (error_information != nullptr)
    {
        auto error_message = exception.what();
        
        // ensure that the string is always null-terminated, even in the case of truncation
        strncpy_s(error_information->message, ImgDoc2ErrorInformation::kMaxMessageLength, error_message, ImgDoc2ErrorInformation::kMaxMessageLength - 1);
    }
}

ImgDoc2ErrorCode MapExceptionToReturnValue(const exception& exception)
{
    if (typeid(exception) == typeid(invalid_argument))
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    // TODO(Jbl) - add additional exception types

    return ImgDoc2_ErrorCode_UnspecifiedError;
}

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

    shared_ptr<imgdoc2::IDoc> imgdoc2;
    try
    {
        imgdoc2 = ClassFactory::CreateNew(reinterpret_cast<ICreateOptions*>(create_options));  // NOLINT(performance-no-int-to-ptr)
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    auto shared_imgdoc_wrappping_object = new SharedPtrWrapper<IDoc>{ imgdoc2 };
    *document = reinterpret_cast<HandleDoc>(shared_imgdoc_wrappping_object);
    int count = imgdoc2.use_count();
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

    int count = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document)->shared_ptr_.use_count();
    auto spReader2d = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document)->shared_ptr_->GetReader2d();   // NOLINT(performance-no-int-to-ptr)
    count = reinterpret_cast<SharedPtrWrapper<IDoc>*>(handle_document)->shared_ptr_.use_count();
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

ImgDoc2ErrorCode CreateOptions_SetFilename(HandleCreateOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->SetFilename(filename_utf8);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_SetUseSpatialIndex(HandleCreateOptions handle, bool use_spatial_index, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->SetUseSpatialIndex(use_spatial_index);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddIndexForDimension(HandleCreateOptions handle, char dimension, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    object->AddIndexForDimension(dimension);
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetFilename(HandleCreateOptions handle, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information)
{
    if (size == nullptr || *size < 1)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)

    const auto& filename = object->GetFilename();
    if (filename_utf8 != nullptr)
    {
        memcpy(filename_utf8, filename.c_str(), min(*size, 1 + filename.length()));
        filename_utf8[*size - 1] = '\0';
    }

    *size = 1 + filename.length();
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetUseSpatialIndex(HandleCreateOptions handle, bool* use_spatial_index, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    const bool b = object->GetUseSpatialIndex();
    if (use_spatial_index != nullptr)
    {
        *use_spatial_index = b;
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddDimension(HandleCreateOptions handle, std::uint8_t  dimension, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    try
    {
        object->AddDimension(dimension);
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_AddIndexedDimension(HandleCreateOptions handle, std::uint8_t  dimension, ImgDoc2ErrorInformation* error_information)
{
    const auto object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    try
    {
        object->AddIndexForDimension(dimension);
    }
    catch (exception& exception)
    {
        FillOutErrorInformation(exception, error_information);
        return MapExceptionToReturnValue(exception);
    }

    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetDimensions(HandleCreateOptions handle, std::uint8_t* dimensions, size_t* elements_count, ImgDoc2ErrorInformation* error_information)
{
    if (elements_count == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto* const object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    const auto dimensions_from_object = object->GetDimensions();

    size_t count = 0;
    for (auto d : dimensions_from_object)
    {
        if (count >= *elements_count)
        {
            break;
        }

        dimensions[count++] = d;
    }

    *elements_count = dimensions_from_object.size();
    return ImgDoc2_ErrorCode_OK;
}

ImgDoc2ErrorCode CreateOptions_GetIndexedDimensions(HandleCreateOptions handle, std::uint8_t* dimensions, size_t* elements_count, ImgDoc2ErrorInformation* error_information)
{
    if (elements_count == nullptr)
    {
        return ImgDoc2_ErrorCode_InvalidArgument;
    }

    auto* const object = reinterpret_cast<ICreateOptions*>(handle);  // NOLINT(performance-no-int-to-ptr)
    const auto dimensions_from_object = object->GetIndexedDimensions();

    size_t count = 0;
    for (auto d : dimensions_from_object)
    {
        if (count >= *elements_count)
        {
            break;
        }

        dimensions[count++] = d;
    }

    *elements_count = dimensions_from_object.size();
    return ImgDoc2_ErrorCode_OK;
}