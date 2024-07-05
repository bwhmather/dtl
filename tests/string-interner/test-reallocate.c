#include "dtl-test.h"

#include <string.h>

#include "dtl-string-interner.h"

int
main(int argc, char **argv) {
    char buffer[12];
    char const *first;
    char const *prev;
    char const *next;
    char const *last;
    int i;
    struct dtl_string_interner *interner;

    (void) argc;
    (void) argv;

    interner = dtl_string_interner_create();

    first = dtl_string_interner_intern(interner, "sentinel");
    dtl_assert(strcmp(first, "sentinel") == 0);

    prev = first;
    for (i = 0; i < 8192; i++) {
        sprintf(buffer, "string %i", i);
        //next = strdup(buffer);

        next = dtl_string_interner_intern(interner, buffer);
        dtl_assert(next != buffer);
        dtl_assert(strcmp(next, buffer) == 0);
        dtl_assert(next != prev);
        dtl_assert(next != first);
        prev = next;
    }

    last = dtl_string_interner_intern(interner, "sentinel");
    dtl_assert(strcmp(first, "sentinel") == 0);
    dtl_assert(last == first);
}
