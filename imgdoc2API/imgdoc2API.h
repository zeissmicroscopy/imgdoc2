#pragma once

#include <cstdint>
#include "importexport.h"
#include "errorcodes.h"
#include "tilecoordinateinterop.h"
#include "logicalpositioninfointerop.h"
#include "queryresultinterop.h"
#include "dimcoordinatequeryclauseinterop.h"
#include "tilebaseinfointerop.h"
#include "statisticsinterop.h"

typedef std::intptr_t ObjectHandle;

static constexpr ObjectHandle kInvalidOjectHandle = 0;

typedef ObjectHandle HandleEnvironmentObject;
typedef ObjectHandle HandleCreateOptions;
typedef ObjectHandle HandleOpenExistingOptions;
typedef ObjectHandle HandleDoc;
typedef ObjectHandle HandleDocRead2D;
typedef ObjectHandle HandleDocWrite2D;

EXTERNAL_API(void)GetStatistics(ImgDoc2StatisticsInterop* statistics_interop);

EXTERNAL_API(HandleEnvironmentObject) CreateEnvironmentObject(
    std::intptr_t user_parameter, 
    void (*pfn_log)(std::intptr_t userparam, int level, const char* szMessage),
    bool (*pfn_is_level_active)(std::intptr_t userparam, int level),
    void (*pfn_report_fatal_error_and_exit)(std::intptr_t userparam, const char* szMessage));
EXTERNAL_API(void) DestroyEnvironmentObject(HandleEnvironmentObject handle);

// factory methods
EXTERNAL_API(HandleCreateOptions) CreateCreateOptions();
EXTERNAL_API(void) DestroyCreateOptions(HandleCreateOptions handle);

EXTERNAL_API(HandleOpenExistingOptions) CreateOpenExistingOptions();
EXTERNAL_API(void) DestroyOpenExistingOptions(HandleOpenExistingOptions handle);

EXTERNAL_API(ImgDoc2ErrorCode) CreateNewDocument(HandleCreateOptions create_options, HandleEnvironmentObject handle_environment_object, HandleDoc* document, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) OpenExistingDocument(HandleOpenExistingOptions open_existing_options, HandleEnvironmentObject handle_environment_object, HandleDoc* document, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(void) DestroyDocument(HandleDoc handle);

EXTERNAL_API(ImgDoc2ErrorCode) IDoc_GetReader2d(HandleDoc handle_document, HandleDocRead2D* document_read2d, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(void) DestroyReader2d(HandleDocRead2D handle);

EXTERNAL_API(ImgDoc2ErrorCode) IDoc_GetWriter2d(HandleDoc handle_document, HandleDocWrite2D* document_write2d, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(void) DestroyWriter2d(HandleDocWrite2D handle);

EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetFilename(HandleCreateOptions handle, const char* filename_utf8, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetUseSpatialIndex(HandleCreateOptions handle, bool use_spatial_index, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddIndexForDimension(HandleCreateOptions handle, char dimension, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_SetUseBlobTable(HandleCreateOptions handle, bool use_blob_table, ImgDoc2ErrorInformation* error_information);

/// Get the property 'filename' from the CreateOptions-object (as an UTF8-encoded string).
/// On input, 'size' specifies the size of the buffer pointed to 'filename_utf8' in bytes. On return, the actual
/// number of bytes required is put here (including the terminating zero character).
/// If 'filename_utf8' is non-null, then at most as many bytes as indicated by 'size' (on input) are written.
/// \param          handle Handle identifying an CreateOptions-object.
/// \param [in,out] filename_utf8        If non-null, the buffer where the string will be placed.
/// \param [in,out] size                On input, the size of the buffer pointed to by 'filename_utf8'; on output the number of bytes actually required.
/// \param [out]    error_information      If non-null, in case of an error, additional information describing the error are put here.
///
/// \returns An errorcode indicating success or failure of the operation.
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetFilename(HandleCreateOptions handle, char* filename_utf8, size_t* size, ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetUseSpatialIndex(HandleCreateOptions handle, bool* use_spatial_index, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetUseBlobTable(HandleCreateOptions handle, bool* create_blob_table, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddDimension(HandleCreateOptions handle, std::uint8_t dimension, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_AddIndexedDimension(HandleCreateOptions handle, std::uint8_t dimension, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetDimensions(HandleCreateOptions handle, std::uint8_t* dimension, size_t* elements_count, ImgDoc2ErrorInformation* error_information);
EXTERNAL_API(ImgDoc2ErrorCode) CreateOptions_GetIndexedDimensions(HandleCreateOptions handle, std::uint8_t* dimension, size_t* elements_count,ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) IDocWrite2d_AddTile(
    HandleDocWrite2D handle, 
    const TileCoordinateInterop* tile_coordinate_interop, 
    const LogicalPositionInfoInterop* logical_position_info_interop,
    const TileBaseInfoInterop* tile_base_info_interop,
    std::uint8_t data_type,
    const void* ptr_data,
    std::uint64_t size_data,
    imgdoc2::dbIndex* result_pk,
    ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) IDocRead2d_Query(
    HandleDocRead2D handle, 
    const DimensionQueryClauseInterop* dim_coordinate_query_clause_interop,
    QueryResultInterop* result, 
    ImgDoc2ErrorInformation* error_information);

EXTERNAL_API(ImgDoc2ErrorCode) IDocRead2d_ReadTileData(
    HandleDocRead2D handle,
    long pk,
    std::intptr_t blob_output_handle,
    bool(LIBIMGDOC2_STDCALL* pfnReserve)(std::intptr_t blob_output_handle, std::uint64_t size),
    bool(LIBIMGDOC2_STDCALL* pfnSetData)(std::intptr_t blob_output_handle, std::uint64_t offset, std::uint64_t size, const void* data),
    ImgDoc2ErrorInformation* error_information);
