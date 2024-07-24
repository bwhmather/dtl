#pragma once

#include <stdio.h>
#include <stdlib.h>

#define dtl_assert(dtl_test_condition)                                         \
    do {                                                                       \
        if (!(dtl_test_condition)) {                                           \
            fprintf(                                                           \
                stderr, "%s:%i %s: Assertion `%s' failed", __FILE__, __LINE__, \
                __func__, #dtl_test_condition);                                \
            abort();                                                           \
        }                                                                      \
    } while (0)
