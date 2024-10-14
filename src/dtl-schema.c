#include "dtl-schema.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-dtype.h"

struct dtl_schema_column {
    char *name;
    enum dtl_dtype dtype;
};

struct dtl_schema {
    size_t num_columns;
    struct dtl_schema_column columns[];
};

struct dtl_schema *
dtl_schema_create(void) {
    return calloc(1, sizeof(struct dtl_schema));
}

struct dtl_schema *
dtl_schema_add_column(struct dtl_schema *schema, char const *name, enum dtl_dtype dtype) {
    assert(schema != NULL);
    assert(name != NULL);
    assert(dtl_dtype_is_array_type(dtype));

    schema->num_columns += 1;
    schema = realloc(schema, sizeof(struct dtl_schema) + schema->num_columns * sizeof(struct dtl_schema_column));
    schema->columns[schema->num_columns - 1].name = strdup(name);
    schema->columns[schema->num_columns - 1].dtype = dtype;

    return schema;
}

size_t
dtl_schema_get_num_columns(struct dtl_schema *schema) {
    assert(schema != NULL);
    return schema->num_columns;
}

char const *
dtl_schema_get_column_name(struct dtl_schema *schema, size_t index) {
    assert(schema != NULL);
    assert(index < schema->num_columns);
    return schema->columns[index].name;
}

enum dtl_dtype
dtl_schema_get_column_dtype(struct dtl_schema *schema, size_t index) {
    assert(schema != NULL);
    assert(index < schema->num_columns);
    return schema->columns[index].dtype;
}

void
dtl_schema_destroy(struct dtl_schema *schema) {
    size_t i;

    if (schema == NULL) {
        return;
    }

    for (i = 0; i < schema->num_columns; i++) {
        free(schema->columns[i].name);
    }
    free(schema);
}
