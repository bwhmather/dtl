#include "dtl-string-array.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char **
dtl_string_array_create(size_t size) {
    return calloc(size, sizeof(char *));
}

void
dtl_string_array_destroy(char **array, size_t size) {
    size_t i;

    if (array == NULL) {
        return;
    }

    for (i = 0; i < size; i++) {
        free(array[i]);
        array[i] = NULL;
    }
    free(array);
}

void
dtl_string_array_set(char **array, size_t index, char const *value) {
    assert(array != NULL);

    dtl_string_array_take(array, index, strdup(value));
}

void
dtl_string_array_take(char **array, size_t index, char *value) {
    assert(array != NULL);

    if (array[index] != NULL) {
        free(array[index]);
    }
    array[index] = value;
}

char const *
dtl_string_array_get(char **array, size_t index) {
    return array[index];
}
