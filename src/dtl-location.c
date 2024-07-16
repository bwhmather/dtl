#include "dtl-location.h"

#include <stdbool.h>
#include <stddef.h>

bool
dtl_location_is_null(struct dtl_location location) {
    return location.offset == SIZE_MAX;
}
