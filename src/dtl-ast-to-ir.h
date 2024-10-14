#pragma once

#include "dtl-ast.h"
#include "dtl-error.h"
#include "dtl-io.h"
#include "dtl-ir.h"
#include "dtl-location.h"


/**
 *
 * - `input_schema_callback`: returns a _reference_ to the input schema identified by the given name.  The
 *   schema it points to must remain valid until the end of the call to `dit_ast_to_ir`.
 */
enum dtl_status
dtl_ast_to_ir(
    struct dtl_ast_node *root,
    struct dtl_ir_graph *graph,
    struct dtl_io_schema *(*input_schema_callback)(char const *, struct dtl_error **, void *),
    void (*column_callback)(char const *, char const *, struct dtl_ir_ref, void *),
    void (*trace_callback)(struct dtl_location, struct dtl_location, char const *, struct dtl_ir_ref, void *),
    void *user_data,
    struct dtl_error **error
);
