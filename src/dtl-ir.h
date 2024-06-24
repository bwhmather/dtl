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

/* --- References ------------------------------------------------------------------------------- */

bool
dtl_ir_ref_is_null(struct dtl_ir_ref ref);

bool
dtl_ir_ref_equal(struct dtl_ir_graph *graph, struct dtl_ir_ref a, struct dtl_ir_ref b);

/* --- Lifecycle -------------------------------------------------------------------------------- */

struct dtl_ir_graph *
dtl_ir_graph_create(size_t nodes_capacity, size_t deps_capacity);

void
dtl_ir_graph_destroy(struct dtl_ir_graph *graph);

/* --- Garbage Collection ----------------------------------------------------------------------- */

/**
 * Marks an expression and all of its direct and indirect dependencies to be retained by the next
 * call to `dtl_ir_graph_gc_collect`.
 */
void
dtl_ir_graph_gc_mark_root(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/**
 * Compacts the expression heap, removing any expression that is not reachable from a marked root.
 * After calling this, external references to nodes must be updated using `dtl_ir_graph_gc_remap`.
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

/* --- Iteration -------------------------------------------------------------------------------- */

void
dtl_ir_graph_for_each_expression(struct dtl_ir_graph *graph, void (*callback)(struct dtl_ir_ref, void *), void *data);

/* === Shape Expressions ======================================================================== */

bool
dtl_ir_is_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

/* --- Import Shape Expressions ----------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_import_shape_expression_create(struct dtl_ir_graph *graph, const char *location);

bool
dtl_ir_is_import_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

const char *
dtl_ir_import_shape_expression_get_location(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

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
dtl_ir_array_expression_get_dtype(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

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

/* --- Import Expressions ----------------------------------------------------------------------- */
// Address is a pointer to a null terminated location identifier followed immediately by a null
// terminated column name.  The location identifier is specific to the importer.
// The expression does not copy or take ownership of the address.  The caller is responsible for
// keeping it alive for the lifetime of the expression and freeing it once finished.

struct dtl_ir_ref
dtl_ir_import_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype, struct dtl_ir_ref shape,
    const char *address
);

bool
dtl_ir_is_import_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

const char *
dtl_ir_import_expression_get_address(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

const char *
dtl_ir_import_expression_get_location(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

const char *
dtl_ir_import_expression_get_column_name(struct dtl_ir_graph *graph, struct dtl_ir_ref ref);

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

/* === Tables =================================================================================== */

struct dtl_ir_table;

struct dtl_ir_table *
dtl_ir_table_create(void);

void
dtl_ir_table_destroy(struct dtl_ir_table *table);

void
dtl_ir_table_add_column(struct dtl_ir_table *table, const char *name, struct dtl_ir_ref ref);

size_t
dtl_ir_table_get_num_columns(struct dtl_ir_table *table);

struct dtl_ir_ref
dtl_ir_table_get_column_expression(struct dtl_ir_table *table, size_t column);

const char *
dtl_ir_table_get_column_name(struct dtl_ir_table *table, size_t column);
