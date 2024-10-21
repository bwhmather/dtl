#pragma once

#include <stdbool.h>

#define DTL_DTYPE_ARRAY_BIT (1 << 14)

enum dtl_dtype {
    DTL_DTYPE_BOOL = 1,
    DTL_DTYPE_INT64,
    DTL_DTYPE_DOUBLE,
    DTL_DTYPE_STRING,
    DTL_DTYPE_INDEX,

    DTL_DTYPE_BOOL_ARRAY = DTL_DTYPE_BOOL | DTL_DTYPE_ARRAY_BIT,
    DTL_DTYPE_INT64_ARRAY = DTL_DTYPE_INT64 | DTL_DTYPE_ARRAY_BIT,
    DTL_DTYPE_DOUBLE_ARRAY = DTL_DTYPE_DOUBLE | DTL_DTYPE_ARRAY_BIT,
    DTL_DTYPE_STRING_ARRAY = DTL_DTYPE_STRING | DTL_DTYPE_ARRAY_BIT,
    DTL_DTYPE_INDEX_ARRAY = DTL_DTYPE_INDEX | DTL_DTYPE_ARRAY_BIT,
};

bool
dtl_dtype_is_scalar_type(enum dtl_dtype type);

bool
dtl_dtype_is_array_type(enum dtl_dtype type);

enum dtl_dtype
dtl_dtype_get_scalar_type(enum dtl_dtype type);

enum dtl_dtype
dtl_dtype_get_array_type(enum dtl_dtype type);
