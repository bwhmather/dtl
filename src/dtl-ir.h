#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct dtl_ir_expression;
struct dtl_ir_graph;

/* --- Lifecycle -------------------------------------------------------------------------------- */

struct dtl_ir_graph*
dtl_ir_graph_create(size_t nodes_capacity, size_t deps_capacity);

void
dtl_ir_graph_destroy(struct dtl_ir_graph* graph);

/* --- Garbage Collection ----------------------------------------------------------------------- */

/**
 * Marks an expression and all of its direct and indirect dependencies to be retained by the next
 * call to `dtl_ir_graph_gc_collect`.
 */
void
dtl_ir_graph_gc_mark_root(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/**
 * Compacts the expression heap, removing any expression that is not reachable from a marked root.
 * After calling this, external references to nodes must be updated using `dtl_ir_graph_gc_remap`.
 * Clears the set of marked roots.
 */
void
dtl_ir_graph_gc_collect(struct dtl_ir_graph* graph);

/**
 * Takes an expression pointer from the previous generation and maps it to its new location
 * post-collection.  Returns NULL if the expression has been collected.  Behaviour is undefined if
 * expression is not from previous generation.
 */
struct dtl_ir_expression*
dtl_ir_graph_gc_remap(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Iteration -------------------------------------------------------------------------------- */

void
dtl_ir_graph_for_each_expression(struct dtl_ir_graph* graph, void (*callback)(struct dtl_ir_expression*, void*), void* data);

/* === Shape Expressions ======================================================================== */

bool
dtl_ir_is_shape_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Import Shape Expressions ----------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_import_shape_expression_create(struct dtl_ir_graph* graph, const char* location);

bool
dtl_ir_is_import_shape_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

const char*
dtl_ir_import_shape_expression_get_location(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Where Shape Expression ------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_where_shape_expression_create(struct dtl_ir_graph* graph, struct dtl_ir_expression* mask);

bool
dtl_ir_is_where_shape_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_where_shape_expression_get_mask(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Join Shape Expressions ------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_join_shape_expression_create(struct dtl_ir_graph* graph, struct dtl_ir_expression* left, struct dtl_ir_expression* right);

bool
dtl_ir_is_join_shape_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_join_shape_expression_get_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_join_shape_expression_get_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* === Array Expressions ======================================================================== */

enum dtl_ir_dtype {
    DTL_DTYPE_BOOL,
    DTL_DTYPE_INT,
    DTL_DTYPE_DOUBLE,
    DTL_DTYPE_TEXT,
    DTL_DTYPE_BYTES,
    DTL_DTYPE_INDEX,
};

bool
dtl_ir_is_array_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

enum dtl_ir_dtype
dtl_ir_array_expression_get_dtype(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_array_expression_get_shape(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Integer Constant Expressions ------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_int_constant_expression_create(
    struct dtl_ir_graph* graph,
    struct dtl_ir_expression* shape,
    uint64_t value
);

bool
dtl_ir_is_int_constant_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

uint64_t
dtl_ir_int_constant_expression_get_value(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Import Expressions ----------------------------------------------------------------------- */
// Address is a pointer to a null terminated location identifier followed immediately by a null
// terminated column name.  The location identifier is specific to the importer.
// The expression does not copy or take ownership of the address.  The caller is responsible for
// keeping it alive for the lifetime of the expression and freeing it once finished.

struct dtl_ir_expression*
dtl_ir_import_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype, struct dtl_ir_expression* shape,
    const char* address
);

bool
dtl_ir_is_import_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

const char*
dtl_ir_import_expression_get_address(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

const char*
dtl_ir_import_expression_get_location(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

const char*
dtl_ir_import_expression_get_column_name(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Where Expressions ------------------------------------------------------------------------ */

struct dtl_ir_expression*
dtl_ir_where_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* source,
    struct dtl_ir_expression* mask
);

bool
dtl_ir_is_where_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_where_expression_get_source(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_where_expression_get_mask(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Pick Expressions ------------------------------------------------------------------------- */
// Returns an array with elements from the source array selected from offsets in the index array.

struct dtl_ir_expression*
dtl_ir_pick_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* source,
    struct dtl_ir_expression* indexes
);

bool
dtl_ir_is_pick_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_pick_expression_get_source(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_pick_expression_get_indexes(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Index Expressions ------------------------------------------------------------------------ */
// Evaluates to an array of indexes into source, sorted so that the values they point to are in
// ascending order.

struct dtl_ir_expression*
dtl_ir_index_expression_create(struct dtl_ir_graph* graph, struct dtl_ir_expression* shape, struct dtl_ir_expression* source);

bool
dtl_ir_is_index_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_index_expression_get_source(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Join Left Expressions -------------------------------------------------------------------- */
// Takes two shapes and returns an index array that selects all values from the left hand side once
// for each index in the right hand shape.

struct dtl_ir_expression*
dtl_ir_join_left_expression_create(
    struct dtl_ir_graph* graph,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left,
    struct dtl_ir_expression* right
);

bool
dtl_ir_is_join_left_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_join_left_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_join_left_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Join Right Expressions ------------------------------------------------------------------- */
// Takes two shapes and returns an index array that selects repeats each value from the right array
// once for each index in the left hand shape.

struct dtl_ir_expression*
dtl_ir_join_right_expression_create(
    struct dtl_ir_graph* graph,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left, struct dtl_ir_expression* right
);

bool
dtl_ir_is_join_right_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_join_right_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_join_right_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Add Expressions -------------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_add_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype, struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left, struct dtl_ir_expression* right
);

bool
dtl_ir_is_add_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_add_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_add_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Subtract Expressions --------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_subtract_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype, struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left, struct dtl_ir_expression* right
);

bool
dtl_ir_is_subtract_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_subtract_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_subtract_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Multiply Expressions --------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_multiply_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype, struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left, struct dtl_ir_expression* right
);

bool
dtl_ir_is_multiply_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_multiply_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_multiply_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

/* --- Divide Expressions ----------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_divide_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype, struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left, struct dtl_ir_expression* right
);

bool
dtl_ir_is_divide_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_divide_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);

struct dtl_ir_expression*
dtl_ir_divide_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression);
