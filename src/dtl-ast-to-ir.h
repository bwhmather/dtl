#pragma once

#include "dtl-ast.h"
#include "dtl-error.h"
#include "dtl-ir.h"
#include "dtl-location.h"
#include "dtl-schema.h"

enum dtl_status
dtl_ast_to_ir(
    struct dtl_ast_node *root,
    struct dtl_ir_graph *graph,
    struct dtl_schema *(*import_callback)(char const *, struct dtl_error **, void *),
    void (*export_callback)(char const *, struct dtl_schema *, struct dtl_ir_ref *, void *),
    void (*trace_callback)(struct dtl_location, struct dtl_location, char const *, struct dtl_ir_ref, void *),
    void *user_data,
    struct dtl_error **error
);
