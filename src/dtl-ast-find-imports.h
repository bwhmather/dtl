#pragma once

#include "dtl-ast.h"

void
dtl_ast_find_imports(struct dtl_ast_node *node, void (*callback)(char const *, void *), void *user_data);
