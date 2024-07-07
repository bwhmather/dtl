#include "dtl-ast.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "dtl-location.h"

struct dtl_ast_node *
dtl_ast_node_create(enum dtl_ast_node_type type) {
    struct dtl_ast_node *node = calloc(
        1, sizeof(struct dtl_ast_node) + 2 * sizeof(struct dtl_ast_node *)
    );
    node->type = type;
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
    return node;
}

struct dtl_ast_node *
dtl_ast_node_append(
    struct dtl_ast_node *container,
    struct dtl_ast_node *child
) {
    assert(container != NULL);

    container->children_length += 1;
    container = realloc(
        container,
        sizeof(struct dtl_ast_node) +
            container->children_length *
                sizeof(struct dtl_ast_node *)
    );

    if (child != NULL) {
        if (container->start.offset == SIZE_MAX) {
            container->start = child->start;
        }
        if (container->end.offset == SIZE_MAX) {
            container->end = child->end;
        }
    }

    container->children[container->children_length - 1] = child;
    return container;
}

struct dtl_ast_node *
dtl_ast_node_get_child(struct dtl_ast_node *container, size_t child_index) {
    assert(container != NULL);
    assert(child_index < container->children_length);
    return container->children[child_index];
}

void
dtl_ast_node_destroy(struct dtl_ast_node *node) {
    if (node == NULL) {
        return;
    }

    for (size_t i = 0; i < node->children_length; i++) {
        dtl_ast_node_destroy(node->children[i]);
    }

    free(node);
}
