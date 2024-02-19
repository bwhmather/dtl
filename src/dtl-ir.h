#pragma once

#include "dtl-graph.h"

#include <stdbool.h>
#include <stddef.h>

#include "dtl-graph.h"

/* === Shape Expressions ======================================================================== */

bool
dtl_is_shape_expression(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Import Shape Expressions ----------------------------------------------------------------- */

struct dtl_node *
dtl_import_shape_expression_create(struct dtl_graph *graph, const char *location);

bool
dtl_is_import_shape_expression(struct dtl_graph *graph, struct dtl_node *expression);

const char *
dtl_import_shape_expression_get_location(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Where Shape Expression ------------------------------------------------------------------- */

struct dtl_node *
dtl_where_shape_expression_create(struct dtl_graph *graph, struct dtl_node *mask);

bool
dtl_is_where_shape_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_where_shape_expression_get_mask(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Join Shape Expressions ------------------------------------------------------------------- */

struct dtl_node *
dtl_join_shape_expression_create(struct dtl_graph *graph, struct dtl_node *left, struct dtl_node *right);

bool
dtl_is_join_shape_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_join_shape_expression_get_left(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_join_shape_expression_get_right(struct dtl_graph *graph, struct dtl_node *expression);


/* === Array Expressions ======================================================================== */

enum dtl_dtype {
    DTL_DTYPE_BOOL,
    DTL_DTYPE_INT,
    DTL_DTYPE_DOUBLE,
    DTL_DTYPE_TEXT,
    DTL_DTYPE_BYTES,
    DTL_DTYPE_INDEX,
};

bool
dtl_is_array_expression(struct dtl_graph *graph, struct dtl_node *expression);

enum dtl_dtype
dtl_array_expression_get_dtype(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_array_expression_get_shape(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Import Expressions ----------------------------------------------------------------------- */

struct dtl_node *
dtl_import_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    const char *location, const char *column_name
);

bool
dtl_is_import_expression(struct dtl_graph *graph, struct dtl_node *expression);

const char *
dtl_import_expression_get_location(struct dtl_graph *graph, struct dtl_node *expression);

const char *
dtl_import_expression_get_column_name(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Where Expressions ------------------------------------------------------------------------ */

struct dtl_node *
dtl_where_expression_create(struct dtl_graph *graph, struct dtl_node *shape, struct dtl_node *source, struct dtl_node *mask);

bool
dtl_is_where_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_where_expression_get_source(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_where_expression_get_mask(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Pick Expressions ------------------------------------------------------------------------- */
// Returns an array with elements from the source array selected from offsets in the index array.

struct dtl_node *
dtl_pick_expression_create(struct dtl_graph *graph, struct dtl_node *shape, struct dtl_node *source, struct dtl_node *indexes);

bool
dtl_is_pick_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_pick_expression_get_source(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_pick_expression_get_indexes(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Index Expressions ------------------------------------------------------------------------ */
// Evaluates to an array of indexes into source, sorted so that the values they point to are in
// ascending order.

struct dtl_node *
dtl_index_expression_create(struct dtl_graph *graph, struct dtl_node *shape, struct dtl_node *source);

bool
dtl_is_index_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_index_expression_get_source(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Join Left Expressions -------------------------------------------------------------------- */
// Takes two shapes and returns an index array that selects all values from the left hand side once
// for each index in the right hand shape.

struct dtl_node *
dtl_join_left_expression_create(
    struct dtl_graph *graph,
    struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
);

bool
dtl_is_join_left_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_join_left_expression_left(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_join_left_expression_right(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Join Right Expressions ------------------------------------------------------------------- */
// Takes two shapes and returns an index array that selects repeats each value from the right array
// once for each index in the left hand shape.

struct dtl_node *
dtl_join_right_expression_create(
    struct dtl_graph *graph,
    struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
);

bool
dtl_is_join_right_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_join_right_expression_left(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_join_right_expression_right(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Add Expressions -------------------------------------------------------------------------- */

struct dtl_node *
dtl_add_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
);

bool
dtl_is_add_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_add_expression_left(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_add_expression_right(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Subtract Expressions --------------------------------------------------------------------- */

struct dtl_node *
dtl_subtract_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
);

bool
dtl_is_subtract_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_subtract_expression_left(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_subtract_expression_right(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Multiply Expressions --------------------------------------------------------------------- */

struct dtl_node *
dtl_multiply_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
);

bool
dtl_is_multiply_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_multiply_expression_left(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_multiply_expression_right(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Divide Expressions ----------------------------------------------------------------------- */

struct dtl_node *
dtl_divide_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
);

bool
dtl_is_divide_expression(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_divide_expression_left(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_divide_expression_right(struct dtl_graph *graph, struct dtl_node *expression);
