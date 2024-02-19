#include "dtl-ir.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "dtl-graph.h"


union {
    int64_t i;
    double d;
    void * p;
} dtl_expression_data;


/* === Shape Expressions ======================================================================== */

enum dtl_node_expression_type {
    DTL_IMPORT_SHAPE_EXPRESSION,
    DTL_WHERE_SHAPE_EXPRESSION,
    DTL_JOIN_SHAPE_EXPRESSION,
};

bool
dtl_expression_is_shape(struct dtl_graph *graph, struct dtl_node *expression) {
    uint32_t tag = dtl_graph_node_get_tag(graph, expression);
    tag &= 1 << 31;
    return (bool) !tag;
}

static enum dtl_node_expression_type
dtl_node_expression_get_expression_type(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_shape_expression(graph, expression));

    uint32_t tag = dtl_graph_node_get_tag(graph, expression);
    tag &= ((uint32_t) 1 << 31) - 1;
    return (enum dtl_node_expression_type) tag;
}

/* --- Import Shape Expressions ----------------------------------------------------------------- */

struct dtl_node *
dtl_import_shape_expression_create(struct dtl_graph *graph, const char *location);

bool
dtl_is_import_shape_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_shape_expression(graph, expression));
    enum dtl_node_expression_type type = dtl_node_expression_get_expression_type(graph, expression);
    return type == DTL_IMPORT_SHAPE_EXPRESSION;
}

const char *
dtl_import_shape_expression_get_location(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Where Shape Expression ------------------------------------------------------------------- */

struct dtl_node *
dtl_where_shape_expression_create(struct dtl_graph *graph, struct dtl_node *mask);

bool
dtl_is_where_shape_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_shape_expression(graph, expression));
    enum dtl_node_expression_type type = dtl_node_expression_get_expression_type(graph, expression);
    return type == DTL_WHERE_SHAPE_EXPRESSION;
}

struct dtl_node *
dtl_where_shape_expression_get_mask(struct dtl_graph *graph, struct dtl_node *expression);


/* --- Join Shape Expressions ------------------------------------------------------------------- */

struct dtl_node *
dtl_join_shape_expression_create(struct dtl_graph *graph, struct dtl_node *left, struct dtl_node *right);

bool
dtl_is_join_shape_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_shape_expression(graph, expression));
    enum dtl_node_expression_type type = dtl_node_expression_get_expression_type(graph, expression);
    return type == DTL_JOIN_SHAPE_EXPRESSION;
}

struct dtl_node *
dtl_join_shape_expression_get_left(struct dtl_graph *graph, struct dtl_node *expression);

struct dtl_node *
dtl_join_shape_expression_get_right(struct dtl_graph *graph, struct dtl_node *expression);


/* === Array Expressions ======================================================================== */

enum dtl_array_expression_type {
    DTL_IMPORT_EXPRESSION,
    DTL_WHERE_EXPRESSION,
    DTL_PICK_EXPRESSION,
    DTL_INDEX_EXPRESSION,
    DTL_JOIN_LEFT_EXPRESSION,
    DTL_JOIN_RIGHT_EXPRESSION,
    DTL_ADD_EXPRESSION,
    DTL_SUBTRACT_EXPRESSION,
    DTL_MULTIPLY_EXPRESSION,
    DTL_DIVIDE_EXPRESSION,
};

bool
dtl_is_array_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    int tag = dtl_graph_node_get_tag(graph, expression);
    tag &= 1 << 30;
    return (bool) tag;
}

static enum dtl_array_expression_type
dtl_array_expression_get_expression_type(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));

    int tag = dtl_graph_node_get_tag(graph, expression);
    tag &= (1 << 30) - 1;
    tag >>= 4;
    return (enum dtl_array_expression_type) tag;
}

enum dtl_dtype
dtl_array_expression_get_dtype(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));

    int tag = dtl_graph_node_get_tag(graph, expression);
    tag &= ((1 << 5) - 1);
    return (enum dtl_dtype) tag;
}

struct dtl_node *
dtl_array_expression_get_shape(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) > 0);
    return dtl_graph_node_get_dependency(graph, expression, 0);
}

/* --- Import Expressions ----------------------------------------------------------------------- */

struct dtl_node *
dtl_import_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    const char *location, const char *column_name
) {
    assert(dtl_is_import_shape_expression(graph, shape));
    assert(dtl_import_shape_expression_get_location(graph, shape) == location);

}

bool
dtl_is_import_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_IMPORT_EXPRESSION;
}

const char *
dtl_import_expression_get_location(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_import_expression(graph, expression));
    return dtl_graph_node_get_data(graph, expression); // TODO
}

const char *
dtl_import_expression_get_column_name(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_import_expression(graph, expression));
    return dtl_graph_node_get_data(graph, expression); // TODO
}


/* --- Where Expressions ------------------------------------------------------------------------ */

struct dtl_node *
dtl_where_expression_create(struct dtl_graph *graph, struct dtl_node *shape, struct dtl_node *source, struct dtl_node *mask) {
    assert(dtl_is_where_shape_expression(graph, shape));
    assert(dtl_is_array_expression(graph, source));
    assert(dtl_is_array_expression(graph, mask));
    assert(dtl_array_expression_get_dtype(graph, mask) == DTL_DTYPE_BOOL);
    assert(dtl_where_shape_expression_get_mask(graph, shape) == mask);
    assert(dtl_array_expression_get_shape(graph, source) == dtl_array_expression_get_shape(graph, mask));

    dtl_create_array_expression_begin(graph, DTL_WHERE_EXPRESSION, dtl_array_expression_get_dtype(graph, source), shape);
    dtl_graph_create_node_add_dependency(graph, source);
    dtl_graph_create_node_add_dependency(graph, mask);
    return dtl_graph_create_node_end(graph);
}

bool
dtl_is_where_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_WHERE_EXPRESSION;
}

struct dtl_node *
dtl_where_expression_get_source(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_where_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 1);
}

struct dtl_node *
dtl_where_expression_get_mask(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_where_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 2);
}


/* --- Pick Expressions ------------------------------------------------------------------------- */
// Returns an array with elements from the source array selected from offsets in the index array.

struct dtl_node *
dtl_pick_expression_create(struct dtl_graph *graph, struct dtl_node *source, struct dtl_node *indexes) {

}

bool
dtl_is_pick_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_PICK_EXPRESSION;
}

struct dtl_node *
dtl_pick_expression_get_source(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_pick_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 1);
}

struct dtl_node *
dtl_pick_expression_get_indexes(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_pick_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 2);
}


/* --- Index Expressions ------------------------------------------------------------------------ */

struct dtl_node *
dtl_index_expression_create(struct dtl_graph *graph, struct dtl_node *source) {

}

bool
dtl_is_index_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_INDEX_EXPRESSION;
}

struct dtl_node *
dtl_index_expression_get_source(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_index_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 2);
    return dtl_graph_node_get_dependency(graph, expression, 1);
}


/* --- Join Left Expressions -------------------------------------------------------------------- */
// Takes two shapes and returns an index array that selects all values from the left hand side once
// for each index in the right hand shape.

struct dtl_node *
dtl_join_left_expression_create(
    struct dtl_graph *graph,
    struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
) {

    assert(dtl_is_join_shape_expression(graph, shape));
    assert(dtl_is_shape_expression(graph, left));
    assert(dtl_is_shape_expression(graph, right));
    assert(dtl_join_shape_expression_get_left(graph, shape) == left);
    assert(dtl_join_shape_expression_get_right(graph, shape) == right);

    dtl_create_array_expression_begin(graph, DTL_JOIN_LEFT_EXPRESSION, DTL_DTYPE_INDEX, shape);
    dtl_graph_create_node_add_dependency(graph, left);
    dtl_graph_create_node_add_dependency(graph, right);
    return dtl_graph_create_node_end(graph);
}

bool
dtl_is_join_left_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_JOIN_LEFT_EXPRESSION;
}

struct dtl_node *
dtl_join_left_expression_left(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_join_left_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 1);
}

struct dtl_node *
dtl_join_left_expression_right(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_join_left_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 2);
}


/* --- Join Right Expressions ------------------------------------------------------------------- */
// Takes two shapes and returns an index array that selects repeats each value from the right array
// once for each index in the left hand shape.

struct dtl_node *
dtl_join_right_expression_create(
    struct dtl_graph *graph,
    struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
) {
    assert(dtl_is_join_shape_expression(graph, shape));
    assert(dtl_is_shape_expression(graph, left));
    assert(dtl_is_shape_expression(graph, right));
    assert(dtl_join_shape_expression_get_left(graph, shape) == left);
    assert(dtl_join_shape_expression_get_right(graph, shape) == right);

    dtl_create_array_expression_begin(graph, DTL_JOIN_RIGHT_EXPRESSION, DTL_DTYPE_INDEX, shape);
    dtl_graph_create_node_add_dependency(graph, left);
    dtl_graph_create_node_add_dependency(graph, right);
    return dtl_graph_create_node_end(graph);
}

bool
dtl_is_join_right_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_JOIN_RIGHT_EXPRESSION;
}

struct dtl_node *
dtl_join_right_expression_left(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_join_right_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 1);
}

struct dtl_node *
dtl_join_right_expression_right(struct dtl_graph *graph, struct dtl_node *expression)  {
    assert(dtl_is_join_right_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 2);
}

/* --- Add Expressions -------------------------------------------------------------------------- */

struct dtl_node *
dtl_add_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
) {
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_is_shape_expression(graph, shape));
    assert(dtl_is_array_expression(graph, left));
    assert(dtl_is_array_expression(graph, right));
    assert(dtl_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_array_expression_get_shape(graph, left) == shape);

    dtl_create_array_expression_begin(graph, DTL_ADD_EXPRESSION, dtype, shape);
    dtl_graph_create_node_add_dependency(graph, left);
    dtl_graph_create_node_add_dependency(graph, right);
    return dtl_graph_create_node_end(graph);
}

bool
dtl_is_add_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_ADD_EXPRESSION;
}

struct dtl_node *
dtl_add_expression_left(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_add_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 1);
}

struct dtl_node *
dtl_add_expression_right(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_add_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 2);
}


/* --- Subtract Expressions --------------------------------------------------------------------- */

struct dtl_node *
dtl_subtract_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
) {
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_is_shape_expression(graph, shape));
    assert(dtl_is_array_expression(graph, left));
    assert(dtl_is_array_expression(graph, right));
    assert(dtl_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_array_expression_get_shape(graph, left) == shape);

    dtl_create_array_expression_begin(graph, DTL_SUBTRACT_EXPRESSION, dtype, shape);
    dtl_graph_create_node_add_dependency(graph, left);
    dtl_graph_create_node_add_dependency(graph, right);
    return dtl_graph_create_node_end(graph);
}

bool
dtl_is_subtract_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_SUBTRACT_EXPRESSION;
}

struct dtl_node *
dtl_subtract_expression_left(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_subtract_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 1);
}

struct dtl_node *
dtl_subtract_expression_right(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_subtract_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 2);
}


/* --- Multiply Expressions --------------------------------------------------------------------- */

struct dtl_node *
dtl_multiply_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
) {
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_is_shape_expression(graph, shape));
    assert(dtl_is_array_expression(graph, left));
    assert(dtl_is_array_expression(graph, right));
    assert(dtl_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_array_expression_get_shape(graph, left) == shape);

    dtl_create_array_expression_begin(graph, DTL_MULTIPLY_EXPRESSION, dtype, shape);
    dtl_graph_create_node_add_dependency(graph, left);
    dtl_graph_create_node_add_dependency(graph, right);
    return dtl_graph_create_node_end(graph);
}

bool
dtl_is_multiply_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_MULTIPLY_EXPRESSION;
}

struct dtl_node *
dtl_multiply_expression_left(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_multiply_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 1);
}

struct dtl_node *
dtl_multiply_expression_right(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_multiply_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 2);
}


/* --- Divide Expressions ----------------------------------------------------------------------- */

struct dtl_node *
dtl_divide_expression_create(
    struct dtl_graph *graph,
    enum dtl_dtype dtype, struct dtl_node *shape,
    struct dtl_node *left, struct dtl_node *right
) {
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_is_shape_expression(graph, shape));
    assert(dtl_is_array_expression(graph, left));
    assert(dtl_is_array_expression(graph, right));
    assert(dtl_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_array_expression_get_shape(graph, left) == shape);

    dtl_create_array_expression_begin(graph, DTL_DIVIDE_EXPRESSION, dtype, shape);
    dtl_graph_create_node_add_dependency(graph, left);
    dtl_graph_create_node_add_dependency(graph, right);
    return dtl_graph_create_node_end(graph);
}

bool
dtl_is_divide_expression(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_array_expression(graph, expression));
    enum dtl_array_expression_type type = dtl_array_expression_get_expression_type(graph, expression);
    return type == DTL_DIVIDE_EXPRESSION;
}

struct dtl_node *
dtl_divide_expression_left(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_divide_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 1);
}

struct dtl_node *
dtl_divide_expression_right(struct dtl_graph *graph, struct dtl_node *expression) {
    assert(dtl_is_divide_expression(graph, expression));
    assert(dtl_graph_node_get_num_dependencies(graph, expression) == 3);
    return dtl_graph_node_get_dependency(graph, expression, 2);
}
