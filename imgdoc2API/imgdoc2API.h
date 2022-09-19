#pragma once

#include <cstdint>
#include "importexport.h"
#include "errorcodes.h"

typedef std::intptr_t ObjectHandle;

static constexpr ObjectHandle kInvalidOjectHandle = 0;

typedef ObjectHandle HandleCreateOptions;
typedef ObjectHandle HandleOpenExistingOptions;
typedef ObjectHandle HandleDoc;
typedef ObjectHandle HandleDocRead2D;

// factory methods
EXTERNAL_API(HandleCreateOptions) CreateCreateOptions();
EXTERNAL_API(void) DestroyCreateOptions(HandleCreateOptions handle);

EXTERNAL_API(HandleOpenExistingOptions) CreateOpenExistingOptions();
EXTERNAL_API(void) DestroyOpenExistingOptions(HandleOpenExistingOptions handle);

EXTERNAL_API(ImgDoc2ErrorCode) CreateNewDocument(HandleCreateOptions create_options, HandleDoc* document, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) OpenExistingDocument(HandleOpenExistingOptions open_existing_options, HandleDoc* document, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(void) DestroyDocument(HandleDoc handle);

EXTERNAL_API(ImgDoc2ErrorCode) IDoc_GetReader2d(HandleDoc handle_document, HandleDocRead2D* document, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(void) DestroyReader2d(HandleDocRead2D handle);

EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetFilename(HandleCreateOptions handle, const char* filename_utf8);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddDimension(HandleCreateOptions handle, char dimension);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetUseSpatialIndex(HandleCreateOptions handle, bool use_spatial_index);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddIndexForDimension(HandleCreateOptions handle, char dimension);

/// Get the property 'filename' from the CreateOptions-object (as an UTF8-encoded string).
/// On input, 'size' specifies the size of the buffer pointed to 'filename_utf8' in bytes. On return, the actual
/// number of bytes required is put here (including the terminating zero character).
/// If 'filename_utf8' is non-null, then at most as many bytes as indicated by 'size' (on input) are written.
/// \param          {HandleCreateOptions} handle Handle identifying an CreateOptions-object.
/// \param [in,out] {char*} filename_utf8        If non-null, the buffer where the string will be placed.
/// \param [in,out] {size_t*} size               On input, the size of the buffer pointed to by 'filename_utf8'; on output the number of bytes actually required.
/// \returns {ImgDoc2ErrorCode} An errorcode indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetFilename(HandleCreateOptions handle, char* filename_utf8, size_t* size);
