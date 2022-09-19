#pragma once

/// Defines an alias representing the error codes for the imgdoc2-module.
/// In general, values greater than zero indicate an error condition; and values less
/// or equal to zero indicate proper operation.
typedef int ImgDoc2ErrorCode;

/// The operation completed successfully.
static constexpr ImgDoc2ErrorCode ImgDoc2_ErrorCode_OK = 0;

/// An invalid argument was supplied to the function.
static constexpr ImgDoc2ErrorCode ImgDoc2_ErrorCode_InvalidArgument = 1;

/// An unspecified error occurred.
static constexpr ImgDoc2ErrorCode ImgDoc2_ErrorCode_UnspecifiedError = 50;

struct ImgDoc2ErrorInformation
{
    // TODO(JBL): idea is to return additional information about an error here
};