#include "dtl-ast-node.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-location.h"

struct dtl_ast_node {
    int tag;
    bool has_children;
    bool has_data;

    struct dtl_location start;
    struct dtl_location end;
};

struct dtl_ast_container_node {
    struct dtl_ast_node base;

    size_t num_children;
    struct dtl_ast_node *children[];
};

struct dtl_ast_data_node {
    struct dtl_ast_node base;

    size_t size;
    char data[];
};

/* === Generic ================================================================================== */

int
dtl_ast_node_get_tag(struct dtl_ast_node *node) {
    assert(node != NULL);

    return node->tag;
}

void
dtl_ast_node_update_bounds(
    struct dtl_ast_node *node, struct dtl_location start, struct dtl_location end
) {
    assert(node != NULL);
    if (!dtl_location_is_null(start)) {
        if (dtl_location_is_null(node->start) || start.offset < node->start.offset) {
            node->start = start;
        }
    }

    if (!dtl_location_is_null(end)) {
        if (dtl_location_is_null(node->end) || end.offset > node->end.offset) {
            node->end = end;
        }
    }
}

struct dtl_location
dtl_ast_node_get_start(struct dtl_ast_node *node) {
    assert(node != NULL);

    return node->start;
}

struct dtl_location
dtl_ast_node_get_end(struct dtl_ast_node *node) {
    assert(node != NULL);

    return node->end;
}

static void
dtl_ast_node_init(struct dtl_ast_node *node, int tag) {
    node->tag = tag;
    node->start = (struct dtl_location){
        .offset = SIZE_MAX,
        .lineno = SIZE_MAX,
        .column = SIZE_MAX
    };
    node->end = (struct dtl_location){
        .offset = SIZE_MAX,
        .lineno = SIZE_MAX,
        .column = SIZE_MAX
    };
}

void
dtl_ast_node_destroy(struct dtl_ast_node *node) {
    struct dtl_ast_container_node *container_node;
    size_t i;

    if (node == NULL) {
        return;
    }

    if (node->has_children) {
        container_node = (struct dtl_ast_container_node *)node;
        for (i = 0; i < container_node->num_children; i++) {
            dtl_ast_node_destroy(container_node->children[i]);
        }
    }

    free(node);
}

/* === Type Specific ============================================================================ */

/* --- Empty Nodes ------------------------------------------------------------------------------ */

struct dtl_ast_node *
dtl_ast_node_create_empty(int tag) {
    struct dtl_ast_node *node;

    node = calloc(1, sizeof(struct dtl_ast_node));
    dtl_ast_node_init(node, tag);

    return node;
}

/* --- Container Nodes -------------------------------------------------------------------------- */

struct dtl_ast_node *
dtl_ast_node_create_with_children(int tag) {
    struct dtl_ast_container_node *container_node;

    container_node = calloc(1, sizeof(struct dtl_ast_container_node));
    dtl_ast_node_init(&container_node->base, tag);
    container_node->base.has_children = true;

    return &container_node->base;
}

struct dtl_ast_node *
dtl_ast_node_append_child(struct dtl_ast_node *node, struct dtl_ast_node *child) {
    struct dtl_ast_container_node *container_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_children(node));

    container_node = (struct dtl_ast_container_node *)node;

    container_node->num_children += 1;
    container_node = realloc(
        container_node,
        sizeof(struct dtl_ast_container_node) +
            container_node->num_children * sizeof(struct dtl_ast_node *)
    );

    container_node->children[container_node->num_children - 1] = child;
    dtl_ast_node_update_bounds(
        &container_node->base,
        dtl_ast_node_get_start(child),
        dtl_ast_node_get_end(child)
    );

    return &container_node->base;
}

bool
dtl_ast_node_has_children(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->has_children;
}

size_t
dtl_ast_node_get_num_children(struct dtl_ast_node *node) {
    struct dtl_ast_container_node *container_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_children(node));

    container_node = (struct dtl_ast_container_node *)node;

    return container_node->num_children;
}

struct dtl_ast_node *
dtl_ast_node_get_child(struct dtl_ast_node *node, size_t index) {
    struct dtl_ast_container_node *container_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_children(node));
    assert(index < dtl_ast_node_get_num_children(node));

    container_node = (struct dtl_ast_container_node *)node;

    return container_node->children[index];
}

/* --- Data Nodes ------------------------------------------------------------------------------- */

struct dtl_ast_node *
dtl_ast_node_create_with_data(int tag, size_t size) {
    struct dtl_ast_data_node *data_node;

    data_node = calloc(1, sizeof(struct dtl_ast_data_node));
    dtl_ast_node_init(&data_node->base, tag);
    data_node->size = size;

    return &data_node->base;
}

bool
dtl_ast_node_has_data(struct dtl_ast_node *node) {
    assert(node != NULL);

    return node->has_data;
}

size_t
dtl_ast_node_get_data_size(struct dtl_ast_node *node) {
    struct dtl_ast_data_node *data_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_data(node));

    data_node = (struct dtl_ast_data_node *)node;

    return data_node->size;
}

void *
dtl_ast_node_get_data(struct dtl_ast_node *node) {
    struct dtl_ast_data_node *data_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_data(node));

    data_node = (struct dtl_ast_data_node *)node;

    return data_node->data;
}
