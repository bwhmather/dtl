#include "dtl-ast.h"

#include <stdarg.h>
#include <stdlib.h>


struct dtl_ast_node *
dtl_ast_node_create(enum dtl_ast_node_type type) {
    struct dtl_ast_node *node = calloc(1, sizeof(struct dtl_ast_node) + 2 * sizeof(struct dtl_ast_node *));
    node->type = type;
    return node;
}


struct dtl_ast_node *
dtl_ast_node_append(struct dtl_ast_node *container, struct dtl_ast_node *child) {
    container->children_length += 1;
    container = realloc(container, sizeof(struct dtl_ast_node) + container->children_length * sizeof(struct dtl_ast_node *));
    if (container->children_length == 1) {
        container->start = child->start;
    }
    container->end = child->end;
    container->children[container->children_length - 1] = child;
    return container;
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
