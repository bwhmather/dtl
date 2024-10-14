#pragma once

#include <stddef.h>

#include "dtl-dtype.h"

struct dtl_schema;

struct dtl_schema *
dtl_schema_create(void);

struct dtl_schema *
dtl_schema_add_column(struct dtl_schema *schema, char const *name, enum dtl_dtype dtype);

size_t
dtl_schema_get_num_columns(struct dtl_schema *schema);

char const *
dtl_schema_get_column_name(struct dtl_schema *schema, size_t index);

enum dtl_dtype
dtl_schema_get_column_dtype(struct dtl_schema *schema, size_t index);

void
dtl_schema_destroy(struct dtl_schema *schema);
