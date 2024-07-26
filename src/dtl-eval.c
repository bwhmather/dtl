#include "dtl-eval.h"

#include <assert.h>
#include <stdio.h>

#include "dtl-ast-to-ir.h"
#include "dtl-ast.h"
#include "dtl-io.h"
#include "dtl-ir-viz.h"
#include "dtl-ir.h"
#include "dtl-location.h"
#include "dtl-parser.h"
#include "dtl-tokenizer.h"

struct dtl_eval_context {
    struct dtl_ir_graph *graph;
};

static void
dtl_eval_ast_to_ir_column_callback(
    char const *table_name,
    char const *column_name,
    struct dtl_ir_ref column_expression,
    void *user_data
) {
    (void)table_name;
    (void)column_name;
    (void)column_expression;
    (void)user_data;
}

static void
dtl_eval_ast_to_ir_trace_callback(
    struct dtl_location start,
    struct dtl_location end,
    char const *column_name,
    struct dtl_ir_ref column_expression,
    void *user_data
) {
    (void)start;
    (void)end;
    (void)column_name;
    (void)column_expression;
    (void)user_data;
}

void
dtl_eval(
    char const *source,
    struct dtl_io_importer *importer,
    struct dtl_io_exporter *exporter,
    struct dtl_io_tracer *tracer
) {
    struct dtl_tokenizer *tokenizer;
    int parse_result;
    struct dtl_ast_node *root;
    struct dtl_ir_graph *graph;
    struct dtl_eval_context context;

    (void)exporter;
    (void)tracer;

    // === Parse Source Code =======================================================================

    tokenizer = dtl_tokenizer_create(source);
    parse_result = dtl_parser_parse(tokenizer, &root);
    assert(parse_result == 0);
    dtl_tokenizer_destroy(tokenizer);

    // === Compile AST to List of Tables Referencing IR Expressions ================================

    graph = dtl_ir_graph_create(1024, 1024);
    context = (struct dtl_eval_context){
        .graph = graph,
    };
    dtl_ast_to_ir(
        root, graph, importer,
        dtl_eval_ast_to_ir_column_callback,
        dtl_eval_ast_to_ir_trace_callback,
        &context
    );

    dtl_ir_viz(stderr, graph);

    // === Optimise IR =============================================================================

    // Optimise regular joins to iteration on one side and lookup on index on other.
    // TODO.

    // Deduplicate IR expressions.
    // TODO.

    // Drop unreachable IR expressions.
    // TODO

    // After this point the expression graph is frozen.  We no longer need to update mappings.

    // === Generate Mappings =======================================================================

    // Generate initial mappings for all reachable expressions pairs.
    // TODO

    // Merge mappings between expressions that aren't in the roots list.
    // TODO

    // === Compile Reachable Expressions to Command List ===========================================
    // TODO

    // === Inject Commands to Export Tables ========================================================
    // TODO

    // === Setup Tracing ===========================================================================
    // TODO

    // === Inject Commands to Collect Arrays After Use =============================================
    // TODO

    // === Evaluate the Command List ===============================================================
    // TODO
}
