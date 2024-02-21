#include "dtl-ir.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct dtl_ir_expression {
    uint32_t tag;
    uint32_t dependencies_end;

    union {
        int64_t as_int;
        double as_double;
        const void* as_pointer;
    } data;
};

struct dtl_ir_graph {
    struct dtl_ir_expression* expressions;
    size_t expressions_length;
    size_t expressions_capacity;

    uint64_t* marks;
    uint32_t* relocations;

    uint32_t* dependencies;
    size_t dependencies_length;
    size_t dependencies_capacity;

    bool writing;
};

/* --- Internal --------------------------------------------------------------------------------- */

static uint32_t
dtl_ir_expression_to_index(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    ptrdiff_t diff = expression - graph->expressions;
    assert(diff >= 0);
    assert((size_t)diff < graph->expressions_length);
    return (uint32_t)diff;
}

static struct dtl_ir_expression*
dtl_ir_index_to_expression(struct dtl_ir_graph* graph, uint32_t index) {
    assert(index < graph->expressions_length);
    return &graph->expressions[index];
}

static void
dtl_ir_scratch_begin(struct dtl_ir_graph* graph) {
    assert(graph != NULL);
    assert(!graph->writing);

    assert(graph->expressions_length < graph->expressions_capacity); // TODO runtime error.

    struct dtl_ir_expression* expression = &graph->expressions[graph->expressions_length];
    memset(expression, 0, sizeof(struct dtl_ir_expression));

    graph->writing = true;
}

static void
dtl_ir_scratch_set_tag(struct dtl_ir_graph* graph, uint32_t tag) {
    assert(graph != NULL);
    assert(graph->writing);

    struct dtl_ir_expression* expression = &graph->expressions[graph->expressions_length];
    assert(expression->tag == 0);

    expression->tag = tag;
}

static void
dtl_ir_scratch_set_int(struct dtl_ir_graph* graph, int64_t value) {
    assert(graph != NULL);
    assert(graph->writing);

    struct dtl_ir_expression* expression = &graph->expressions[graph->expressions_length];

    expression->data.as_int = value;
}

static void
dtl_ir_scratch_set_double(struct dtl_ir_graph* graph, double value) {
    assert(graph != NULL);
    assert(graph->writing);

    struct dtl_ir_expression* expression = &graph->expressions[graph->expressions_length];

    expression->data.as_double = value;
}

static void
dtl_ir_scratch_set_pointer(struct dtl_ir_graph* graph, const void* value) {
    assert(graph != NULL);
    assert(graph->writing);

    struct dtl_ir_expression* expression = &graph->expressions[graph->expressions_length];

    expression->data.as_pointer = value;
}

static void
dtl_ir_scratch_add_dependency(struct dtl_ir_graph* graph, struct dtl_ir_expression* dependency) {
    assert(graph != NULL);
    assert(graph->writing);

    assert(graph->dependencies_length < graph->dependencies_capacity); // TODO runtime error.

    uint32_t dependency_index = dtl_ir_expression_to_index(graph, dependency);
    graph->dependencies[graph->dependencies_length] = dependency_index;

    graph->dependencies_length += 1;
    graph->expressions[graph->expressions_length].dependencies_end += 1;
}

static struct dtl_ir_expression*
dtl_ir_scratch_end(struct dtl_ir_graph* graph) {
    assert(graph != NULL);
    assert(graph->writing);

    graph->writing = false;
    return &graph->expressions[graph->expressions_length++];
}

static size_t
dtl_ir_expression_get_num_dependencies(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    uint32_t expression_index = dtl_ir_expression_to_index(graph, expression);

    uint32_t start = 0;
    if (expression_index > 0) {
        start = graph->expressions[expression_index - 1].dependencies_end;
    }

    uint32_t end = graph->expressions[expression_index].dependencies_end;

    assert(end >= start);
    return end - start;
}

static struct dtl_ir_expression*
dtl_ir_expression_get_dependency(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression, size_t dependency_index) {
    uint32_t expression_index = dtl_ir_expression_to_index(graph, expression);

    uint32_t start = 0;
    if (expression_index > 0) {
        start = graph->expressions[expression_index - 1].dependencies_end;
    }

    uint32_t end = graph->expressions[expression_index].dependencies_end;

    assert(end >= start);
    assert(end - start > dependency_index);
    assert(start + dependency_index < graph->dependencies_capacity);

    return dtl_ir_index_to_expression(graph, graph->dependencies[start + dependency_index]);
}

/* --- Lifecycle -------------------------------------------------------------------------------- */

struct dtl_ir_graph*
dtl_ir_graph_create(size_t expressions_capacity, size_t dependencies_capacity) {
    assert(expressions_capacity <= UINT32_MAX);
    assert(dependencies_capacity <= UINT32_MAX);

    struct dtl_ir_graph* graph = NULL;
    struct dtl_ir_expression* expressions = NULL;
    uint64_t* marks = NULL;
    uint32_t* relocations = NULL;
    uint32_t* dependencies = NULL;

    expressions = calloc(sizeof(struct dtl_ir_expression), expressions_capacity);
    if (expressions == NULL) {
        goto error;
    }

    marks = calloc(sizeof(uint64_t), (expressions_capacity + 63) / 64);
    if (marks == NULL) {
        goto error;
    }

    relocations = calloc(sizeof(uint32_t), expressions_capacity);
    if (relocations == NULL) {
        goto error;
    }

    dependencies = calloc(sizeof(uint32_t), dependencies_capacity);
    if (dependencies == NULL) {
        goto error;
    }

    graph = calloc(1, sizeof(struct dtl_ir_graph));
    if (graph == NULL) {
        goto error;
    }
    graph->expressions = expressions;
    graph->expressions_capacity = expressions_capacity;
    graph->marks = marks;
    graph->relocations = relocations;
    graph->dependencies = dependencies;
    graph->dependencies_capacity = dependencies_capacity;

    return graph;

error:
    free(dependencies);
    free(relocations);
    free(marks);
    free(expressions);
    free(graph);

    return NULL;
}

void
dtl_ir_graph_destroy(struct dtl_ir_graph* graph);

/* --- Garbage Collection ----------------------------------------------------------------------- */

void
dtl_ir_graph_gc_mark_root(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    (void)graph;
    (void)expression;
    // TODO.
}

void
dtl_ir_graph_gc_collect(struct dtl_ir_graph* graph) {
    (void)graph;
    // TODO.
}

struct dtl_ir_expression*
dtl_ir_graph_gc_remap(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    (void)graph;
    return expression;
}

/* === Shape Expressions ======================================================================== */

enum dtl_ir_shape_expression_type {
    DTL_IMPORT_SHAPE_EXPRESSION,
    DTL_WHERE_SHAPE_EXPRESSION,
    DTL_JOIN_SHAPE_EXPRESSION,
};

static uint32_t
dtl_ir_shape_expression_tag(enum dtl_ir_shape_expression_type type) {
    return (uint32_t)type;
}

static enum dtl_ir_shape_expression_type
dtl_ir_shape_expression_get_type(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_shape_expression(graph, expression));

    return (enum dtl_ir_shape_expression_type)expression->tag;
}

bool
dtl_ir_is_shape_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    (void)graph;
    return (expression->tag & (1 << 31)) == 0;
}

/* --- Import Shape Expressions ----------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_import_shape_expression_create(struct dtl_ir_graph* graph, const char* location) {
    assert(graph != NULL);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_shape_expression_tag(DTL_IMPORT_SHAPE_EXPRESSION));
    dtl_ir_scratch_set_pointer(graph, location);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_import_shape_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);

    if (!dtl_ir_is_shape_expression(graph, expression)) {
        return false;
    }

    enum dtl_ir_shape_expression_type type = dtl_ir_shape_expression_get_type(graph, expression);
    if (type != DTL_IMPORT_SHAPE_EXPRESSION) {
        return false;
    }

    return true;
}

const char*
dtl_ir_import_shape_expression_get_location(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_import_shape_expression(graph, expression));

    return expression->data.as_pointer;
}

/* --- Where Shape Expression ------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_where_shape_expression_create(struct dtl_ir_graph* graph, struct dtl_ir_expression* mask) {
    assert(graph != NULL);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_shape_expression_tag(DTL_WHERE_SHAPE_EXPRESSION));
    dtl_ir_scratch_add_dependency(graph, mask);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_where_shape_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);

    if (!dtl_ir_is_shape_expression(graph, expression)) {
        return false;
    }

    enum dtl_ir_shape_expression_type type = dtl_ir_shape_expression_get_type(graph, expression);
    if (type != DTL_WHERE_SHAPE_EXPRESSION) {
        return false;
    }

    return true;
}

struct dtl_ir_expression*
dtl_ir_where_shape_expression_get_mask(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_where_shape_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 1);

    return dtl_ir_expression_get_dependency(graph, expression, 0);
}

/* --- Join Shape Expressions ------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_join_shape_expression_create(struct dtl_ir_graph* graph, struct dtl_ir_expression* left, struct dtl_ir_expression* right);

bool
dtl_ir_is_join_shape_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);

    if (!dtl_ir_is_shape_expression(graph, expression)) {
        return false;
    }

    enum dtl_ir_shape_expression_type type = dtl_ir_shape_expression_get_type(graph, expression);
    if (type != DTL_JOIN_SHAPE_EXPRESSION) {
        return false;
    }

    return true;
}

struct dtl_ir_expression*
dtl_ir_join_shape_expression_get_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_shape_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 2);

    return dtl_ir_expression_get_dependency(graph, expression, 0);
}

struct dtl_ir_expression*
dtl_ir_join_shape_expression_get_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_shape_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 2);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

/* === Array Expressions ======================================================================== */

enum dtl_ir_array_expression_type {
    DTL_CONSTANT_EXPRESSION,
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

static uint32_t
dtl_ir_array_expression_tag(enum dtl_ir_array_expression_type type, enum dtl_ir_dtype dtype) {
    uint32_t tag = 1 << 31;
    tag &= type << 4;
    tag &= dtype;
    return tag;
}

static enum dtl_ir_array_expression_type
dtl_ir_array_expression_get_type(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    int tag = expression->tag;
    tag &= ((uint32_t)1 << 31) - 1;
    tag >>= 4;
    return (enum dtl_ir_array_expression_type)tag;
}

bool
dtl_ir_is_array_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    (void)graph;
    return (expression->tag & (1 << 31)) != 0;
}

enum dtl_ir_dtype
dtl_ir_array_expression_get_dtype(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    (void)graph;
    return expression->tag & ((1 << 5) - 1);
}

struct dtl_ir_expression*
dtl_ir_array_expression_get_shape(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) > 0);

    return dtl_ir_expression_get_dependency(graph, expression, 0);
}

/* --- Integer Constant Expressions ------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_int_constant_expression_create(
    struct dtl_ir_graph* graph,
    struct dtl_ir_expression* shape,
    uint64_t value
) {
    assert(graph != NULL);
    assert(dtl_ir_is_shape_expression(graph, shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_CONSTANT_EXPRESSION, DTL_DTYPE_INT));
    dtl_ir_scratch_set_int(graph, value);
    dtl_ir_scratch_add_dependency(graph, shape);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_int_constant_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    if (type != DTL_WHERE_EXPRESSION) {
        return false;
    }

    enum dtl_ir_dtype dtype = dtl_ir_array_expression_get_dtype(graph, expression);
    if (dtype != DTL_DTYPE_INT) {
        return false;
    }

    return true;
}

uint64_t
dtl_ir_int_constant_expression_get_value(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_int_constant_expression(graph, expression));

    return expression->data.as_int;
}

/* --- Double Constant Expressions ------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_double_constant_expression_create(
    struct dtl_ir_graph* graph,
    struct dtl_ir_expression* shape,
    double value
) {
    assert(graph != NULL);
    assert(dtl_ir_is_shape_expression(graph, shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_CONSTANT_EXPRESSION, DTL_DTYPE_INT));
    dtl_ir_scratch_set_double(graph, value);
    dtl_ir_scratch_add_dependency(graph, shape);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_double_constant_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    if (type != DTL_WHERE_EXPRESSION) {
        return false;
    }

    enum dtl_ir_dtype dtype = dtl_ir_array_expression_get_dtype(graph, expression);
    if (dtype != DTL_DTYPE_DOUBLE) {
        return false;
    }

    return true;
}

uint64_t
dtl_ir_double_constant_expression_get_value(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_int_constant_expression(graph, expression));

    return expression->data.as_double;
}

/* --- Import Expressions ----------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_import_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype,
    struct dtl_ir_expression* shape,
    const char* address
) {
    assert(graph != NULL);
    assert(dtl_ir_is_import_shape_expression(graph, shape));
    assert(strcmp(dtl_ir_import_shape_expression_get_location(graph, shape), address) == 0);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_IMPORT_EXPRESSION, dtype));
    dtl_ir_scratch_set_pointer(graph, address);
    dtl_ir_scratch_add_dependency(graph, shape);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_import_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_IMPORT_EXPRESSION;
}

const char*
dtl_ir_import_expression_get_address(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_import_expression(graph, expression));

    const char* address = expression->data.as_pointer;
    return address;
}

const char*
dtl_ir_import_expression_get_location(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_import_expression(graph, expression));

    const char* address = expression->data.as_pointer;
    return address;
}

const char*
dtl_ir_import_expression_get_column_name(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_import_expression(graph, expression));

    const char* address = expression->data.as_pointer;
    return address + strlen(address) + 1;
}

/* --- Where Expressions ------------------------------------------------------------------------ */

struct dtl_ir_expression*
dtl_ir_where_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* source,
    struct dtl_ir_expression* mask
) {
    assert(graph != NULL);
    assert(dtl_ir_is_where_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, source));
    assert(dtl_ir_is_array_expression(graph, mask));
    assert(dtl_ir_array_expression_get_dtype(graph, mask) == DTL_DTYPE_BOOL);
    assert(dtl_ir_where_shape_expression_get_mask(graph, shape) == mask);
    assert(dtl_ir_array_expression_get_shape(graph, source) == dtl_ir_array_expression_get_shape(graph, mask));
    assert(dtl_ir_array_expression_get_dtype(graph, source) == dtype);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_WHERE_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, source);
    dtl_ir_scratch_add_dependency(graph, mask);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_where_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_WHERE_EXPRESSION;
}

struct dtl_ir_expression*
dtl_ir_where_expression_get_source(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_where_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_expression*
dtl_ir_where_expression_get_mask(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_where_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Pick Expressions ------------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_pick_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* source,
    struct dtl_ir_expression* indexes
) {
    assert(graph != NULL);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, source));
    assert(dtl_ir_array_expression_get_dtype(graph, source) == dtype);
    assert(dtl_ir_is_array_expression(graph, indexes));
    assert(dtl_ir_array_expression_get_dtype(graph, indexes) == DTL_DTYPE_INDEX);
    assert(dtl_ir_array_expression_get_shape(graph, indexes) == shape);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_PICK_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, source);
    dtl_ir_scratch_add_dependency(graph, indexes);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_pick_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_PICK_EXPRESSION;
}

struct dtl_ir_expression*
dtl_ir_pick_expression_get_source(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_pick_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_expression*
dtl_ir_pick_expression_get_indexes(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_pick_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Index Expressions ------------------------------------------------------------------------ */

struct dtl_ir_expression*
dtl_ir_index_expression_create(
    struct dtl_ir_graph* graph,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* source
) {
    assert(graph != NULL);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, source));
    assert(dtl_ir_array_expression_get_shape(graph, source) == shape);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_INDEX_EXPRESSION, DTL_DTYPE_INDEX));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, source);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_index_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_INDEX_EXPRESSION;
}

struct dtl_ir_expression*
dtl_ir_index_expression_get_source(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_index_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 2);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

/* --- Join Left Expressions -------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_join_left_expression_create(
    struct dtl_ir_graph* graph,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left,
    struct dtl_ir_expression* right
) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_shape_expression(graph, shape));
    assert(dtl_ir_is_shape_expression(graph, left));
    assert(dtl_ir_is_shape_expression(graph, right));
    assert(dtl_ir_join_shape_expression_get_left(graph, shape) == left);
    assert(dtl_ir_join_shape_expression_get_right(graph, shape) == right);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_JOIN_LEFT_EXPRESSION, DTL_DTYPE_INDEX));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_join_left_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(dtl_ir_is_array_expression(graph, expression));
    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_JOIN_LEFT_EXPRESSION;
}

struct dtl_ir_expression*
dtl_ir_join_left_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(dtl_ir_is_join_left_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);
    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_expression*
dtl_ir_join_left_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(dtl_ir_is_join_left_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);
    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Join Right Expressions ------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_join_right_expression_create(
    struct dtl_ir_graph* graph,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left,
    struct dtl_ir_expression* right
) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_shape_expression(graph, shape));
    assert(dtl_ir_is_shape_expression(graph, left));
    assert(dtl_ir_is_shape_expression(graph, right));
    assert(dtl_ir_join_shape_expression_get_left(graph, shape) == left);
    assert(dtl_ir_join_shape_expression_get_right(graph, shape) == right);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_JOIN_RIGHT_EXPRESSION, DTL_DTYPE_INDEX));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_join_right_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_JOIN_RIGHT_EXPRESSION;
}

struct dtl_ir_expression*
dtl_ir_join_right_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_right_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_expression*
dtl_ir_join_right_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_right_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Add Expressions -------------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_add_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left,
    struct dtl_ir_expression* right
) {
    assert(graph != NULL);
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, left));
    assert(dtl_ir_is_array_expression(graph, right));
    assert(dtl_ir_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_ir_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_ir_array_expression_get_shape(graph, left) == shape);
    assert(dtl_ir_array_expression_get_shape(graph, right) == shape);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_ADD_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_add_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_ADD_EXPRESSION;
}

struct dtl_ir_expression*
dtl_ir_add_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_add_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_expression*
dtl_ir_add_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_add_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Subtract Expressions --------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_subtract_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype, struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left, struct dtl_ir_expression* right
) {
    assert(graph != NULL);
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, left));
    assert(dtl_ir_is_array_expression(graph, right));
    assert(dtl_ir_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_ir_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_ir_array_expression_get_shape(graph, left) == shape);
    assert(dtl_ir_array_expression_get_shape(graph, right) == shape);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_SUBTRACT_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_subtract_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_SUBTRACT_EXPRESSION;
}

struct dtl_ir_expression*
dtl_ir_subtract_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_subtract_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_expression*
dtl_ir_subtract_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_subtract_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Multiply Expressions --------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_multiply_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype, struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left, struct dtl_ir_expression* right
) {
    assert(graph != NULL);
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, left));
    assert(dtl_ir_is_array_expression(graph, right));
    assert(dtl_ir_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_ir_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_ir_array_expression_get_shape(graph, left) == shape);
    assert(dtl_ir_array_expression_get_shape(graph, right) == shape);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_MULTIPLY_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_multiply_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_MULTIPLY_EXPRESSION;
}

struct dtl_ir_expression*
dtl_ir_multiply_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_multiply_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_expression*
dtl_ir_multiply_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_multiply_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Divide Expressions ----------------------------------------------------------------------- */

struct dtl_ir_expression*
dtl_ir_divide_expression_create(
    struct dtl_ir_graph* graph,
    enum dtl_ir_dtype dtype,
    struct dtl_ir_expression* shape,
    struct dtl_ir_expression* left,
    struct dtl_ir_expression* right
) {
    assert(graph != NULL);
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, left));
    assert(dtl_ir_is_array_expression(graph, right));
    assert(dtl_ir_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_ir_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_ir_array_expression_get_shape(graph, left) == shape);
    assert(dtl_ir_array_expression_get_shape(graph, right) == shape);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_DIVIDE_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_divide_expression(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_DIVIDE_EXPRESSION;
}

struct dtl_ir_expression*
dtl_ir_divide_expression_left(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_divide_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_expression*
dtl_ir_divide_expression_right(struct dtl_ir_graph* graph, struct dtl_ir_expression* expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_divide_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* === Tables =================================================================================== */

struct dtl_ir_column {
    char *name;
    struct dtl_ir_expression *expression;
};

struct dtl_ir_table {
    struct dtl_ir_column *columns;
    size_t columns_length;
};

void
dtl_ir_table_destroy(struct dtl_ir_table *table) {
    assert(table != NULL);

    for (size_t i = 0; i < table->columns_length; i++) {
        free(table->columns[i].name);
    }
    free(table->columns);
    free(table);
}

struct dtl_ir_table *
dtl_ir_table_create(void) {
    struct dtl_ir_table *table = calloc(sizeof(struct dtl_ir_table), 1);
    return table;
}

void
dtl_ir_table_add_column(struct dtl_ir_table *table, const char *name, struct dtl_ir_expression *expression) {
    assert(table != NULL);

    table->columns = realloc(table->columns, table->columns_length + 1);
    assert(table->columns != NULL);

    struct dtl_ir_column *column = &table->columns[table->columns_length];

    column->name = strdup(name);
    assert(column->name != NULL);

    column->expression = expression;
}

size_t
dtl_ir_table_get_num_columns(struct dtl_ir_table *table) {
    assert(table != NULL);
    return table->columns_length;
}

struct dtl_ir_expression *
dtl_ir_table_get_column_expression(struct dtl_ir_table *table, size_t column);

const char *
dtl_ir_table_get_column_name(struct dtl_ir_table *table, size_t column);

