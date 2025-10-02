#ifndef __RETURN_H__
#define __RETURN_H__

#include "stdio.h"
#include "stdint.h"

/// Default return type
typedef int32_t def;

#define STR_OKE                "OKE"
#define STR_ERR                "ERR"
#define STR_ERR_NULL           "ERR_NULL"
#define STR_ERR_MALLOC_FAILED  "ERR_MALLOC_FAILED"
#define STR_ERR_TIMEOUT        "ERR_TIMEOUT"
#define STR_ERR_BUSY           "ERR_BUSY"
#define STR_ERR_INVALID_ARG    "ERR_INVALID_ARG"
#define STR_ERR_OVERFLOW       "ERR_OVERFLOW"
#define STR_ERR_UNDERFLOW      "ERR_UNDERFLOW"
#define STR_ERR_NOT_FOUND      "ERR_NOT_FOUND"
#define STR_ERR_ALREADY_EXISTS "ERR_ALREADY_EXISTS"
#define STR_ERR_NOT_IMPLEMENTED "ERR_NOT_IMPLEMENTED"
#define STR_ERR_UNSUPPORTED    "ERR_UNSUPPORTED"
#define STR_ERR_IO             "ERR_IO"
#define STR_ERR_PERMISSION     "ERR_PERMISSION"
#define STR_ERR_CRC            "ERR_CRC"
#define STR_ERR_INIT_FAILED    "ERR_INIT_FAILED"

/// Standardized return status codes for functions.
enum DEFAULT_RETURN_STATUS {
    OKE                 = 0,   ///< Success
    ERR                 = -1,  ///< Generic error
    ERR_NULL            = -2,  ///< Null pointer passed
    ERR_MALLOC_FAILED   = -3,  ///< Memory allocation failed
    ERR_TIMEOUT         = -4,  ///< Timeout occurred
    ERR_BUSY            = -5,  ///< Resource/device is busy
    ERR_INVALID_ARG     = -6,  ///< Invalid argument
    ERR_OVERFLOW        = -7,  ///< Buffer/variable overflow
    ERR_UNDERFLOW       = -8,  ///< Buffer/variable underflow
    ERR_NOT_FOUND       = -9,  ///< Resource not found
    ERR_ALREADY_EXISTS  = -10, ///< Resource already exists
    ERR_NOT_IMPLEMENTED = -11, ///< Functionality not implemented
    ERR_UNSUPPORTED     = -12, ///< Unsupported operation
    ERR_IO              = -13, ///< Input/Output error
    ERR_PERMISSION      = -14, ///< Permission denied
    ERR_CRC             = -15, ///< CRC check failed (data corrupted)
    ERR_INIT_FAILED     = -16, ///< Initialization failed
};

#endif