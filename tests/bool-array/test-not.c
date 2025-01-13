#include "dtl-test.h"

#include "dtl-bool-array.h"
#include <string.h>

int
main(int argc, char **argv) {
    size_t size = 71;
    void *input;
    void *output;
    size_t i;
    uint64_t sum;
    bool value;

    (void) argc;
    (void) argv;

    input = dtl_bool_array_create(size);
    for (i = 0; i < size; i++) {
        dtl_bool_array_set(input, i, i % 2 ? true : false);
    }
    sum = dtl_bool_array_sum(input, size);
    dtl_assert(sum == 35);

    output = dtl_bool_array_create(size);
    dtl_bool_array_not(input, size, output);

    for (i = 0; i < size; i++) {
        value = dtl_bool_array_get(output, i);
        dtl_assert(value == i % 2 ? false : true);
    }
    sum = dtl_bool_array_sum(output, size);
    dtl_assert(sum == 36);
}
