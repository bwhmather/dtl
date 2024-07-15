#include "dtl-ast.h"

#include "dtl-ast-node.h"

enum dtl_ast_node_type
dtl_ast_node_get_type(struct dtl_ast_node *node) {
    return (enum dtl_ast_node_type)dtl_ast_node_get_tag(node);
}

int
dtl_ast_node_get_class(struct dtl_ast_node *node) {
    enum dtl_ast_node_type type = dtl_ast_node_get_type(node);
    return (type & DTL_AST_CLASS_MASK);
}
