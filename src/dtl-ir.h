#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "dtl-dtype.h"

struct dtl_ir_expression;
struct dtl_ir_graph;

struct dtl_ir_ref {
    int space : 2;
    uint32_t offset : 30;
};

#define DTL_IR_NULL_REF ((struct dtl_ir_ref){0})

/* --- References ------------------------------------------------------------------------------- */

bool
dtl_ir_ref_is_null(struct dtl_ir_ref ref);

bool
dtl_ir_ref_equal(struct dtl_ir_graph *graph, struct dtl_ir_ref a, struct dtl_ir_ref b);

size_t
dtl_ir_ref_to_index(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_index_to_ref(struct dtl_ir_graph *graph, size_t index);

/* --- Generic ---------------------------------------------------------------------------------- */

enum dtl_dtype
dtl_ir_expression_get_dtype(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

size_t
dtl_ir_expression_get_num_dependencies(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_expression_get_dependency(struct dtl_ir_graph *graph, struct dtl_ir_ref ref, size_t);

size_t
dtl_ir_graph_get_size(struct dtl_ir_graph *graph);

/* --- Lifecycle -------------------------------------------------------------------------------- */

struct dtl_ir_graph *
dtl_ir_graph_create(size_t nodes_capacity, size_t deps_capacity);

void
dtl_ir_graph_destroy(struct dtl_ir_graph *graph);

/* --- Strings ---------------------------------------------------------------------------------- */

/**
 * Intern a string using the graph's embedded string interner.  The returned string will be valid
 * for the lifetime of the graph and can be compared by pointer to other strings from the same graph.
 */
char const *
dtl_ir_graph_intern(struct dtl_ir_graph *graph, char const *input);

/* --- Transformation --------------------------------------------------------------------------- */

void
dtl_ir_graph_transform(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref (*callback)(struct dtl_ir_graph *graph, struct dtl_ir_ref, void *),
    void *data
);

/* --- Garbage Collection ----------------------------------------------------------------------- */

/**
 * Marks an expression and all of its direct and indirect dependencies to be retained by the next
 * call to `dtl_ir_graph_gc_collect`.
 */
void
dtl_ir_graph_gc_mark_root(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/**
 * Compacts the expression heap, removing any expression that is not reachable from a marked root.
 * After calling this, external references to nodes must be updated using `dtl_ir_graph_remap_ref`.
 * Clears the set of marked roots.
 */
void
dtl_ir_graph_gc_collect(struct dtl_ir_graph *graph);

/**
 * Takes an expression pointer from the previous generation and maps it to its new location
 * post-collection.  Returns NULL if the expression has been collected.  Behaviour is undefined if
 * expression is not from previous generation.
 */
void
dtl_ir_graph_remap_ref(struct dtl_ir_graph *graph, struct dtl_ir_ref *ref);

/* === Shape Expressions ======================================================================== */

bool
dtl_ir_is_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Table Shape Expressions ------------------------------------------------------------------ */

struct dtl_ir_ref
dtl_ir_table_shape_expression_create(struct dtl_ir_graph *graph, struct dtl_ir_ref table);

bool
dtl_ir_is_table_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_table_shape_expression_get_table(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Where Shape Expression ------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_where_shape_expression_create(struct dtl_ir_graph *graph, struct dtl_ir_ref mask);

bool
dtl_ir_is_where_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_where_shape_expression_get_mask(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Join Shape Expressions ------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_join_shape_expression_create(struct dtl_ir_graph *graph, struct dtl_ir_ref left, struct dtl_ir_ref right);

bool
dtl_ir_is_join_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_join_shape_expression_get_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_join_shape_expression_get_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* === Array Expressions ======================================================================== */

bool
dtl_ir_is_array_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

enum dtl_dtype
dtl_ir_array_expression_get_element_dtype(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_array_expression_get_shape(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Integer Constant Expressions ------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_int_constant_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    uint64_t value
);

bool
dtl_ir_is_int_constant_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

uint64_t
dtl_ir_int_constant_expression_get_value(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Double Constant Expressions ------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_double_constant_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    double value
);

bool
dtl_ir_is_double_constant_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression);

uint64_t
dtl_ir_double_constant_expression_get_value(struct dtl_ir_graph *graph, struct dtl_ir_ref expression);

/* --- Open Table Expressions ------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_open_table_expression_create(struct dtl_ir_graph *graph, const char *path);

bool
dtl_ir_is_open_table_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

char const *
dtl_ir_open_table_expression_get_path(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Read Column Expressions ------------------------------------------------------------------ */

struct dtl_ir_ref
dtl_ir_read_column_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref table,
    const char *column_name
);

bool
dtl_ir_is_read_column_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_read_column_expression_get_table(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

const char *
dtl_ir_read_column_expression_get_column_name(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Where Expressions ------------------------------------------------------------------------ */

struct dtl_ir_ref
dtl_ir_where_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref source,
    struct dtl_ir_ref mask
);

bool
dtl_ir_is_where_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_where_expression_get_source(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_where_expression_get_mask(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Pick Expressions ------------------------------------------------------------------------- */
// Returns an array with elements from the source array selected from offsets in the index array.

struct dtl_ir_ref
dtl_ir_pick_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref source,
    struct dtl_ir_ref indexes
);

bool
dtl_ir_is_pick_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_pick_expression_get_source(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_pick_expression_get_indexes(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Index Expressions ------------------------------------------------------------------------ */
// Evaluates to an array of indexes into source, sorted so that the values they point to are in
// ascending order.

struct dtl_ir_ref
dtl_ir_index_expression_create(struct dtl_ir_graph *graph, struct dtl_ir_ref shape, struct dtl_ir_ref source);

bool
dtl_ir_is_index_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_index_expression_get_source(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Join Left Expressions -------------------------------------------------------------------- */
// Takes two shapes and returns an index array that selects all values from the left hand side once
// for each index in the right hand shape.

struct dtl_ir_ref
dtl_ir_join_left_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
);

bool
dtl_ir_is_join_left_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_join_left_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_join_left_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Join Right Expressions ------------------------------------------------------------------- */
// Takes two shapes and returns an index array that selects repeats each value from the right array
// once for each index in the left hand shape.

struct dtl_ir_ref
dtl_ir_join_right_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left, struct dtl_ir_ref right
);

bool
dtl_ir_is_join_right_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_join_right_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_join_right_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Equal-to Expressions --------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_equal_to_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
);

bool
dtl_ir_is_equal_to_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_equal_to_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_equal_to_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Less-than Expressions -------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_less_than_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
);

bool
dtl_ir_is_less_than_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_less_than_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_less_than_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Less-than-or-equal-to Expressions -------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_less_than_or_equal_to_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
);

bool
dtl_ir_is_less_than_or_equal_to_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_less_than_or_equal_to_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_less_than_or_equal_to_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Greater-than expressions ----------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_greater_than_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
);

bool
dtl_ir_is_greater_than_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_greater_than_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_greater_than_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Greater-than-or-equal-to Expressions ----------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_greater_than_or_equal_to_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
);

bool
dtl_ir_is_greater_than_or_equal_to_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_greater_than_or_equal_to_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_greater_than_or_equal_to_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Add Expressions -------------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_add_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype, struct dtl_ir_ref shape,
    struct dtl_ir_ref left, struct dtl_ir_ref right
);

bool
dtl_ir_is_add_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_add_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_add_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Subtract Expressions --------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_subtract_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype, struct dtl_ir_ref shape,
    struct dtl_ir_ref left, struct dtl_ir_ref right
);

bool
dtl_ir_is_subtract_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_subtract_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_subtract_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Multiply Expressions --------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_multiply_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype, struct dtl_ir_ref shape,
    struct dtl_ir_ref left, struct dtl_ir_ref right
);

bool
dtl_ir_is_multiply_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_multiply_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_multiply_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Divide Expressions ----------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_divide_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype, struct dtl_ir_ref shape,
    struct dtl_ir_ref left, struct dtl_ir_ref right
);

bool
dtl_ir_is_divide_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_divide_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

struct dtl_ir_ref
dtl_ir_divide_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);
