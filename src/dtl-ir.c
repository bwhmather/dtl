#include "dtl-ir.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-dtype.h"

struct dtl_ir_expression {
    uint32_t tag;
    uint32_t dependencies_end;

    union {
        int64_t as_int;
        double as_double;
        void const *as_pointer;
    } value;
};

struct dtl_ir_space {
    int id;

    struct dtl_ir_expression *expressions;
    size_t expressions_length;
    size_t expressions_capacity;

    struct dtl_ir_ref *dependencies;
    size_t dependencies_length;
    size_t dependencies_capacity;
};

struct dtl_ir_graph {
    struct dtl_ir_space from_space;
    struct dtl_ir_space to_space;

    uint64_t *marks;
    struct dtl_ir_ref *relocations;

    bool transforming : 1;
    bool writing : 1;
};

/* --- References ------------------------------------------------------------------------------- */

bool
dtl_ir_ref_is_null(struct dtl_ir_ref ref) {
    return ref.space == 0 && ref.offset == 0;
}

bool
dtl_ir_ref_equal(struct dtl_ir_graph *graph, struct dtl_ir_ref a, struct dtl_ir_ref b) {
    assert(graph != NULL);
    if (graph->transforming) {
        dtl_ir_graph_remap_ref(graph, &a);
        dtl_ir_graph_remap_ref(graph, &b);
    }

    assert(a.space == graph->to_space.id);
    assert(b.space == graph->to_space.id);

    return a.offset == b.offset;
}

/* --- Internal --------------------------------------------------------------------------------- */

static void
dtl_ir_scratch_begin(struct dtl_ir_graph *graph) {
    struct dtl_ir_expression *expression;

    assert(graph != NULL);
    assert(!graph->writing);

    assert(graph->to_space.expressions_length < graph->to_space.expressions_capacity); // TODO runtime error.

    expression = &graph->to_space.expressions[graph->to_space.expressions_length];
    memset(expression, 0, sizeof(struct dtl_ir_expression));

    graph->writing = true;
}

static void
dtl_ir_scratch_set_tag(struct dtl_ir_graph *graph, uint32_t tag) {
    struct dtl_ir_expression *expression;

    assert(graph != NULL);
    assert(graph->writing);

    expression = &graph->to_space.expressions[graph->to_space.expressions_length];
    assert(expression->tag == 0);

    expression->tag = tag;
}

static void
dtl_ir_scratch_set_int(struct dtl_ir_graph *graph, int64_t value) {
    struct dtl_ir_expression *expression;

    assert(graph != NULL);
    assert(graph->writing);

    expression = &graph->to_space.expressions[graph->to_space.expressions_length];

    expression->value.as_int = value;
}

static void
dtl_ir_scratch_set_double(struct dtl_ir_graph *graph, double value) {
    struct dtl_ir_expression *expression;

    assert(graph != NULL);
    assert(graph->writing);

    expression = &graph->to_space.expressions[graph->to_space.expressions_length];

    expression->value.as_double = value;
}

static void
dtl_ir_scratch_set_pointer(struct dtl_ir_graph *graph, void const *value) {
    struct dtl_ir_expression *expression;

    assert(graph != NULL);
    assert(graph->writing);

    expression = &graph->to_space.expressions[graph->to_space.expressions_length];

    expression->value.as_pointer = value;
}

static void
dtl_ir_scratch_add_dependency(struct dtl_ir_graph *graph, struct dtl_ir_ref dependency) {
    assert(graph != NULL);
    assert(graph->writing);

    assert(graph->to_space.dependencies_length < graph->to_space.dependencies_capacity); // TODO runtime error.

    if (graph->transforming) {
        dtl_ir_graph_remap_ref(graph, &dependency);
    }
    assert(dependency.space == graph->to_space.id);

    graph->to_space.dependencies[graph->to_space.dependencies_length] = dependency;

    graph->to_space.dependencies_length += 1;
    graph->to_space.expressions[graph->to_space.expressions_length].dependencies_end += 1;
}

static struct dtl_ir_ref
dtl_ir_scratch_end(struct dtl_ir_graph *graph) {
    struct dtl_ir_ref result;

    assert(graph != NULL);
    assert(graph->writing);

    graph->writing = false;

    graph->to_space.expressions_length += 1;

    result.space = graph->to_space.id;
    result.offset = graph->to_space.expressions_length;
    return result;
}

static uint32_t
dtl_ir_space_get_expression_tag(
    struct dtl_ir_space *space,
    struct dtl_ir_ref expression
) {
    assert(expression.space == space->id);
    assert(expression.offset < space->expressions_length);

    return space->expressions[expression.offset].tag;
}

static uint32_t
dtl_ir_expression_get_tag(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref expression
) {
    if (graph->transforming) {
        dtl_ir_graph_remap_ref(graph, &expression);
    }
    return dtl_ir_space_get_expression_tag(&graph->to_space, expression);
}

static int64_t
dtl_ir_space_get_expression_value_as_int(
    struct dtl_ir_space *space,
    struct dtl_ir_ref expression
) {
    assert(expression.space == space->id);
    assert(expression.offset < space->expressions_length);

    return space->expressions[expression.offset].value.as_int;
}

static int64_t
dtl_ir_expression_get_value_as_int(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref expression
) {
    if (graph->transforming) {
        dtl_ir_graph_remap_ref(graph, &expression);
    }
    return dtl_ir_space_get_expression_value_as_int(&graph->to_space, expression);
}

static double
dtl_ir_space_get_expression_value_as_double(
    struct dtl_ir_space *space,
    struct dtl_ir_ref expression
) {
    assert(expression.space == space->id);
    assert(expression.offset < space->expressions_length);

    return space->expressions[expression.offset].value.as_double;
}

static double
dtl_ir_expression_get_value_as_double(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref expression
) {
    if (graph->transforming) {
        dtl_ir_graph_remap_ref(graph, &expression);
    }
    return dtl_ir_space_get_expression_value_as_double(&graph->to_space, expression);
}

static void const *
dtl_ir_space_get_expression_value_as_pointer(
    struct dtl_ir_space *space,
    struct dtl_ir_ref expression
) {
    assert(expression.space == space->id);
    assert(expression.offset < space->expressions_length);

    return space->expressions[expression.offset].value.as_pointer;
}

static void const *
dtl_ir_expression_get_value_as_pointer(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref expression
) {
    if (graph->transforming) {
        dtl_ir_graph_remap_ref(graph, &expression);
    }
    return dtl_ir_space_get_expression_value_as_pointer(&graph->to_space, expression);
}

static size_t
dtl_ir_space_get_expression_num_dependencies(
    struct dtl_ir_space *space,
    struct dtl_ir_ref expression
) {
    uint32_t expression_index;
    uint32_t start;
    uint32_t end;

    assert(expression.space == space->id);

    expression_index = expression.offset;
    assert(expression_index <= space->expressions_length);

    start = 0;
    if (expression_index > 0) {
        start = space->expressions[expression_index - 1].dependencies_end;
    }

    end = space->expressions[expression_index].dependencies_end;

    assert(end >= start);
    return end - start;
}

static size_t
dtl_ir_expression_get_num_dependencies(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref expression
) {
    if (graph->transforming) {
        dtl_ir_graph_remap_ref(graph, &expression);
    }
    return dtl_ir_space_get_expression_num_dependencies(&graph->to_space, expression);
}

static struct dtl_ir_ref
dtl_ir_space_get_expression_dependency(
    struct dtl_ir_space *space,
    struct dtl_ir_ref expression,
    size_t dependency_index
) {
    uint32_t expression_index;
    uint32_t start = 0;
    uint32_t end;

    assert(expression.space == space->id);

    expression_index = expression.offset;
    assert(expression_index <= space->expressions_length);

    start = 0;
    if (expression_index > 0) {
        start = space->expressions[expression_index - 1].dependencies_end;
    }

    end = space->expressions[expression_index].dependencies_end;

    assert(end >= start);
    assert(end - start > dependency_index);
    assert(start + dependency_index < space->dependencies_capacity);

    return space->dependencies[start + dependency_index];
}

static struct dtl_ir_ref
dtl_ir_expression_get_dependency(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref expression,
    size_t dependency_index
) {
    if (graph->transforming) {
        dtl_ir_graph_remap_ref(graph, &expression);
    }
    return dtl_ir_space_get_expression_dependency(&graph->to_space, expression, dependency_index);
}

/* --- Lifecycle -------------------------------------------------------------------------------- */

struct dtl_ir_graph *
dtl_ir_graph_create(size_t expressions_capacity, size_t dependencies_capacity) {
    struct dtl_ir_graph *graph = NULL;
    struct dtl_ir_expression *to_expressions = NULL;
    struct dtl_ir_ref *to_dependencies = NULL;
    struct dtl_ir_expression *from_expressions = NULL;
    struct dtl_ir_ref *from_dependencies = NULL;
    uint64_t *marks = NULL;
    struct dtl_ir_ref *relocations = NULL;

    assert(expressions_capacity < (1 << 30));
    assert(dependencies_capacity < (1 << 30));

    to_expressions = calloc(expressions_capacity, sizeof(struct dtl_ir_expression));
    if (to_expressions == NULL) {
        goto error;
    }

    to_dependencies = calloc(dependencies_capacity, sizeof(uint32_t));
    if (to_dependencies == NULL) {
        goto error;
    }

    from_expressions = calloc(expressions_capacity, sizeof(struct dtl_ir_expression));
    if (to_expressions == NULL) {
        goto error;
    }

    from_dependencies = calloc(dependencies_capacity, sizeof(uint32_t));
    if (to_dependencies == NULL) {
        goto error;
    }

    marks = calloc((expressions_capacity + 63) / 64, sizeof(uint64_t));
    if (marks == NULL) {
        goto error;
    }

    relocations = calloc(expressions_capacity, sizeof(uint32_t));
    if (relocations == NULL) {
        goto error;
    }

    graph = calloc(1, sizeof(struct dtl_ir_graph));
    if (graph == NULL) {
        goto error;
    }
    graph->to_space.id = 1;
    graph->to_space.expressions = to_expressions;
    graph->to_space.expressions_capacity = expressions_capacity;
    graph->to_space.dependencies = to_dependencies;
    graph->to_space.dependencies_capacity = dependencies_capacity;

    graph->from_space.id = 2;
    graph->from_space.expressions = from_expressions;
    graph->from_space.expressions_capacity = expressions_capacity;
    graph->from_space.dependencies = from_dependencies;
    graph->from_space.dependencies_capacity = dependencies_capacity;

    graph->marks = marks;
    graph->relocations = relocations;

    return graph;

error:
    free(to_expressions);
    free(to_dependencies);
    free(from_expressions);
    free(from_dependencies);
    free(relocations);
    free(marks);
    free(graph);

    return NULL;
}

void
dtl_ir_graph_destroy(struct dtl_ir_graph *graph) {
    free(graph->to_space.expressions);
    free(graph->to_space.dependencies);
    free(graph->from_space.expressions);
    free(graph->from_space.dependencies);
    free(graph->relocations);
    free(graph->marks);
    free(graph);
}

/* --- Transformation --------------------------------------------------------------------------- */
void
dtl_ir_graph_transform(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref (*callback)(struct dtl_ir_graph *graph, struct dtl_ir_ref, void *),
    void *data
) {
    struct dtl_ir_space tmp_space;
    struct dtl_ir_ref old_ref;
    struct dtl_ir_ref new_ref;
    size_t i;
    size_t j;
    struct dtl_ir_ref dep_ref;

    assert(!graph->transforming);

    graph->transforming = true;

    tmp_space = graph->from_space;
    graph->from_space = graph->to_space;
    graph->to_space = tmp_space;

    graph->relocations = realloc(graph->relocations, graph->from_space.expressions_length * sizeof(struct dtl_ir_ref));
    assert(graph->relocations != NULL);

    for (i = 0; i < graph->from_space.expressions_length; i++) {
        old_ref.space = graph->from_space.id;
        old_ref.offset = i;

        new_ref = callback(graph, old_ref, data);

        if (memcmp(&old_ref, &new_ref, sizeof(struct dtl_ir_ref)) == 0) {
            dtl_ir_scratch_begin(graph);
            dtl_ir_scratch_set_tag(graph, dtl_ir_space_get_expression_tag(&graph->from_space, old_ref));
            for (j = 0; j < dtl_ir_space_get_expression_num_dependencies(&graph->from_space, old_ref); j++) {
                dep_ref = dtl_ir_space_get_expression_dependency(&graph->from_space, old_ref, j);
                dtl_ir_graph_remap_ref(graph, &dep_ref);
                dtl_ir_scratch_add_dependency(graph, dep_ref);
            }

            new_ref = dtl_ir_scratch_end(graph);
        }
        dtl_ir_graph_remap_ref(graph, &new_ref);

        graph->relocations[i] = new_ref;
    }

    graph->transforming = false;
}

/* --- Garbage Collection ----------------------------------------------------------------------- */

void
dtl_ir_graph_gc_mark_root(struct dtl_ir_graph *graph, struct dtl_ir_ref ref) {
    (void)graph;
    (void)ref;
    // TODO.
}

void
dtl_ir_graph_gc_collect(struct dtl_ir_graph *graph) {
    (void)graph;
    // TODO.
}

void
dtl_ir_graph_remap_ref(struct dtl_ir_graph *graph, struct dtl_ir_ref *ref) {
    if (dtl_ir_ref_is_null(*ref)) {
        return;
    }

    if (ref->space == graph->from_space.id) {
        *ref = graph->relocations[ref->offset];
    }

    assert(ref->space == graph->to_space.id);
}

/* === Shape Expressions ======================================================================== */

enum dtl_ir_shape_expression_type {
    DTL_IMPORT_SHAPE_EXPRESSION = 1,
    DTL_WHERE_SHAPE_EXPRESSION,
    DTL_JOIN_SHAPE_EXPRESSION,
};

static uint32_t
dtl_ir_shape_expression_tag(enum dtl_ir_shape_expression_type type) {
    return (uint32_t)type;
}

static enum dtl_ir_shape_expression_type
dtl_ir_shape_expression_get_type(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    uint32_t tag;

    assert(graph != NULL);
    assert(dtl_ir_is_shape_expression(graph, expression));

    tag = dtl_ir_expression_get_tag(graph, expression);

    return (enum dtl_ir_shape_expression_type)tag;
}

bool
dtl_ir_is_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    uint32_t tag;

    assert(graph != NULL);

    tag = dtl_ir_expression_get_tag(graph, expression);

    return (tag & (1 << 31)) == 0;
}

/* --- Import Shape Expressions ----------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_import_shape_expression_create(struct dtl_ir_graph *graph, const char *location) {
    assert(graph != NULL);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_shape_expression_tag(DTL_IMPORT_SHAPE_EXPRESSION));
    dtl_ir_scratch_set_pointer(graph, location);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_import_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
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

const char *
dtl_ir_import_shape_expression_get_location(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_import_shape_expression(graph, expression));

    return dtl_ir_expression_get_value_as_pointer(graph, expression);
}

/* --- Where Shape Expression ------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_where_shape_expression_create(struct dtl_ir_graph *graph, struct dtl_ir_ref mask) {
    assert(graph != NULL);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_shape_expression_tag(DTL_WHERE_SHAPE_EXPRESSION));
    dtl_ir_scratch_add_dependency(graph, mask);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_where_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
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

struct dtl_ir_ref
dtl_ir_where_shape_expression_get_mask(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_where_shape_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 1);

    return dtl_ir_expression_get_dependency(graph, expression, 0);
}

/* --- Join Shape Expressions ------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_join_shape_expression_create(struct dtl_ir_graph *graph, struct dtl_ir_ref left, struct dtl_ir_ref right);

bool
dtl_ir_is_join_shape_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
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

struct dtl_ir_ref
dtl_ir_join_shape_expression_get_left(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_shape_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 2);

    return dtl_ir_expression_get_dependency(graph, expression, 0);
}

struct dtl_ir_ref
dtl_ir_join_shape_expression_get_right(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
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
dtl_ir_array_expression_tag(enum dtl_ir_array_expression_type type, enum dtl_dtype dtype) {
    uint32_t tag = 1 << 31;
    tag &= type << 4;
    tag &= dtype;
    return tag;
}

static enum dtl_ir_array_expression_type
dtl_ir_array_expression_get_type(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    uint32_t tag;

    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    tag = dtl_ir_expression_get_tag(graph, expression);
    tag &= ((uint32_t)1 << 31) - 1;
    tag >>= 4;
    return (enum dtl_ir_array_expression_type)tag;
}

bool
dtl_ir_is_array_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    uint32_t tag = dtl_ir_expression_get_tag(graph, expression);
    return (tag & (1 << 31)) != 0;
}

enum dtl_dtype
dtl_ir_array_expression_get_dtype(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    uint32_t tag = dtl_ir_expression_get_tag(graph, expression);
    return tag & ((1 << 5) - 1);
}

struct dtl_ir_ref
dtl_ir_array_expression_get_shape(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) > 0);

    return dtl_ir_expression_get_dependency(graph, expression, 0);
}

/* --- Integer Constant Expressions ------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_int_constant_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
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
dtl_ir_is_int_constant_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    if (type != DTL_WHERE_EXPRESSION) {
        return false;
    }

    enum dtl_dtype dtype = dtl_ir_array_expression_get_dtype(graph, expression);
    if (dtype != DTL_DTYPE_INT) {
        return false;
    }

    return true;
}

uint64_t
dtl_ir_int_constant_expression_get_value(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_int_constant_expression(graph, expression));

    return dtl_ir_expression_get_value_as_int(graph, expression);
}

/* --- Double Constant Expressions ------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_double_constant_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
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
dtl_ir_is_double_constant_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    if (type != DTL_WHERE_EXPRESSION) {
        return false;
    }

    enum dtl_dtype dtype = dtl_ir_array_expression_get_dtype(graph, expression);
    if (dtype != DTL_DTYPE_DOUBLE) {
        return false;
    }

    return true;
}

uint64_t
dtl_ir_double_constant_expression_get_value(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_int_constant_expression(graph, expression));

    return dtl_ir_expression_get_value_as_double(graph, expression);
}

/* --- Import Expressions ----------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_import_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype,
    struct dtl_ir_ref shape,
    const char *address
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
dtl_ir_is_import_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_IMPORT_EXPRESSION;
}

const char *
dtl_ir_import_expression_get_address(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_import_expression(graph, expression));

    const char *address = dtl_ir_expression_get_value_as_pointer(graph, expression);
    return address;
}

const char *
dtl_ir_import_expression_get_location(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_import_expression(graph, expression));

    const char *address = dtl_ir_expression_get_value_as_pointer(graph, expression);
    return address;
}

const char *
dtl_ir_import_expression_get_column_name(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_import_expression(graph, expression));

    const char *address = dtl_ir_expression_get_value_as_pointer(graph, expression);
    return address + strlen(address) + 1;
}

/* --- Where Expressions ------------------------------------------------------------------------ */

struct dtl_ir_ref
dtl_ir_where_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref source,
    struct dtl_ir_ref mask
) {
    assert(graph != NULL);
    assert(dtl_ir_is_where_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, source));
    assert(dtl_ir_is_array_expression(graph, mask));
    assert(dtl_ir_array_expression_get_dtype(graph, mask) == DTL_DTYPE_BOOL);
    assert(dtl_ir_ref_equal(graph, dtl_ir_where_shape_expression_get_mask(graph, shape), mask));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, source), dtl_ir_array_expression_get_shape(graph, mask)));
    assert(dtl_ir_array_expression_get_dtype(graph, source) == dtype);

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_WHERE_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, source);
    dtl_ir_scratch_add_dependency(graph, mask);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_where_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_WHERE_EXPRESSION;
}

struct dtl_ir_ref
dtl_ir_where_expression_get_source(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_where_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_ref
dtl_ir_where_expression_get_mask(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_where_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Pick Expressions ------------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_pick_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref source,
    struct dtl_ir_ref indexes
) {
    assert(graph != NULL);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, source));
    assert(dtl_ir_array_expression_get_dtype(graph, source) == dtype);
    assert(dtl_ir_is_array_expression(graph, indexes));
    assert(dtl_ir_array_expression_get_dtype(graph, indexes) == DTL_DTYPE_INDEX);
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, indexes), shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_PICK_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, source);
    dtl_ir_scratch_add_dependency(graph, indexes);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_pick_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_PICK_EXPRESSION;
}

struct dtl_ir_ref
dtl_ir_pick_expression_get_source(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_pick_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_ref
dtl_ir_pick_expression_get_indexes(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_pick_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Index Expressions ------------------------------------------------------------------------ */

struct dtl_ir_ref
dtl_ir_index_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref source
) {
    assert(graph != NULL);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, source));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, source), shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_INDEX_EXPRESSION, DTL_DTYPE_INDEX));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, source);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_index_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_INDEX_EXPRESSION;
}

struct dtl_ir_ref
dtl_ir_index_expression_get_source(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_index_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 2);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

/* --- Join Left Expressions -------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_join_left_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_shape_expression(graph, shape));
    assert(dtl_ir_is_shape_expression(graph, left));
    assert(dtl_ir_is_shape_expression(graph, right));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, left), shape));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, right), shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_JOIN_LEFT_EXPRESSION, DTL_DTYPE_INDEX));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_join_left_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(dtl_ir_is_array_expression(graph, expression));
    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_JOIN_LEFT_EXPRESSION;
}

struct dtl_ir_ref
dtl_ir_join_left_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(dtl_ir_is_join_left_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);
    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_ref
dtl_ir_join_left_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(dtl_ir_is_join_left_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);
    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Join Right Expressions ------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_join_right_expression_create(
    struct dtl_ir_graph *graph,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_shape_expression(graph, shape));
    assert(dtl_ir_is_shape_expression(graph, left));
    assert(dtl_ir_is_shape_expression(graph, right));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, left), shape));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, right), shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_JOIN_RIGHT_EXPRESSION, DTL_DTYPE_INDEX));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_join_right_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_JOIN_RIGHT_EXPRESSION;
}

struct dtl_ir_ref
dtl_ir_join_right_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_right_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_ref
dtl_ir_join_right_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_join_right_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Add Expressions -------------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_add_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
) {
    assert(graph != NULL);
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, left));
    assert(dtl_ir_is_array_expression(graph, right));
    assert(dtl_ir_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_ir_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, left), shape));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, right), shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_ADD_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_add_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_ADD_EXPRESSION;
}

struct dtl_ir_ref
dtl_ir_add_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_add_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_ref
dtl_ir_add_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_add_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Subtract Expressions --------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_subtract_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype, struct dtl_ir_ref shape,
    struct dtl_ir_ref left, struct dtl_ir_ref right
) {
    assert(graph != NULL);
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, left));
    assert(dtl_ir_is_array_expression(graph, right));
    assert(dtl_ir_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_ir_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, left), shape));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, right), shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_SUBTRACT_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_subtract_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_SUBTRACT_EXPRESSION;
}

struct dtl_ir_ref
dtl_ir_subtract_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_subtract_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_ref
dtl_ir_subtract_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_subtract_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Multiply Expressions --------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_multiply_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype, struct dtl_ir_ref shape,
    struct dtl_ir_ref left, struct dtl_ir_ref right
) {
    assert(graph != NULL);
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, left));
    assert(dtl_ir_is_array_expression(graph, right));
    assert(dtl_ir_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_ir_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, left), shape));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, right), shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_MULTIPLY_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_multiply_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_MULTIPLY_EXPRESSION;
}

struct dtl_ir_ref
dtl_ir_multiply_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_multiply_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_ref
dtl_ir_multiply_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_multiply_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* --- Divide Expressions ----------------------------------------------------------------------- */

struct dtl_ir_ref
dtl_ir_divide_expression_create(
    struct dtl_ir_graph *graph,
    enum dtl_dtype dtype,
    struct dtl_ir_ref shape,
    struct dtl_ir_ref left,
    struct dtl_ir_ref right
) {
    assert(graph != NULL);
    assert(dtype == DTL_DTYPE_INT || dtype == DTL_DTYPE_DOUBLE);
    assert(dtl_ir_is_shape_expression(graph, shape));
    assert(dtl_ir_is_array_expression(graph, left));
    assert(dtl_ir_is_array_expression(graph, right));
    assert(dtl_ir_array_expression_get_dtype(graph, left) == dtype);
    assert(dtl_ir_array_expression_get_dtype(graph, right) == dtype);
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, left), shape));
    assert(dtl_ir_ref_equal(graph, dtl_ir_array_expression_get_shape(graph, right), shape));

    dtl_ir_scratch_begin(graph);
    dtl_ir_scratch_set_tag(graph, dtl_ir_array_expression_tag(DTL_DIVIDE_EXPRESSION, dtype));
    dtl_ir_scratch_add_dependency(graph, shape);
    dtl_ir_scratch_add_dependency(graph, left);
    dtl_ir_scratch_add_dependency(graph, right);
    return dtl_ir_scratch_end(graph);
}

bool
dtl_ir_is_divide_expression(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_array_expression(graph, expression));

    enum dtl_ir_array_expression_type type = dtl_ir_array_expression_get_type(graph, expression);
    return type == DTL_DIVIDE_EXPRESSION;
}

struct dtl_ir_ref
dtl_ir_divide_expression_left(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_divide_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 1);
}

struct dtl_ir_ref
dtl_ir_divide_expression_right(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    assert(graph != NULL);
    assert(dtl_ir_is_divide_expression(graph, expression));
    assert(dtl_ir_expression_get_num_dependencies(graph, expression) == 3);

    return dtl_ir_expression_get_dependency(graph, expression, 2);
}

/* === Tables =================================================================================== */

struct dtl_ir_column {
    char *name;
    struct dtl_ir_ref expression;
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
    struct dtl_ir_table *table = calloc(1, sizeof(struct dtl_ir_table));
    return table;
}

void
dtl_ir_table_add_column(struct dtl_ir_table *table, const char *name, struct dtl_ir_ref expression) {
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

struct dtl_ir_ref
dtl_ir_table_get_column_expression(struct dtl_ir_table *table, size_t column);

const char *
dtl_ir_table_get_column_name(struct dtl_ir_table *table, size_t column);
