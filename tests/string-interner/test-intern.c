#include "dtl-test.h"

#include "dtl-string-interner.h"
#include <string.h>

int
main(int argc, char **argv) {
    char const *input = "string";
    char const *output_1;
    char const *output_2;
    char const *output_3;
    struct dtl_string_interner *interner;

    (void) argc;
    (void) argv;

    interner = dtl_string_interner_create();
    dtl_assert(interner != NULL);

    output_1 = dtl_string_interner_intern(interner, input);
    dtl_assert(output_1 != NULL);
    dtl_assert(output_1 != input);
    dtl_assert(strcmp(output_1, input) == 0);

    output_2 = dtl_string_interner_intern(interner, input);
    dtl_assert(output_2 == output_1);

    output_3 = dtl_string_interner_intern(interner, output_1);
    dtl_assert(output_3 == output_1);

    dtl_string_interner_destroy(interner);
}
