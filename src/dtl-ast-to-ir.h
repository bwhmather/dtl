#pragma once

#include "dtl-ast.h"
#include "dtl-io.h"
#include "dtl-ir.h"
#include "dtl-location.h"

void
dtl_ast_to_ir(
    struct dtl_ast_node *root,
    struct dtl_ir_graph *graph,
    struct dtl_io_importer *importer,
    void (*column_callback)(char const *, char const *, struct dtl_ir_ref, void *),
    void (*trace_callback)(struct dtl_location, struct dtl_location, char const *, struct dtl_ir_ref, void *),
    void *user_data
);
