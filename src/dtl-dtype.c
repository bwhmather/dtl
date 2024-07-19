#include "dtl-dtype.h"

#include <stdbool.h>

bool
dtl_dtype_is_scalar_type(enum dtl_dtype type) {
    return !(type & DTL_DTYPE_ARRAY_BIT);
}

bool
dtl_dtype_is_array_type(enum dtl_dtype type) {
    return type & DTL_DTYPE_ARRAY_BIT;
}

enum dtl_dtype
dtl_dtype_get_scalar_type(enum dtl_dtype type) {
    return type & (~DTL_DTYPE_ARRAY_BIT);
}

enum dtl_dtype
dtl_dtype_get_array_type(enum dtl_dtype type) {
    return type | DTL_DTYPE_ARRAY_BIT;
}
