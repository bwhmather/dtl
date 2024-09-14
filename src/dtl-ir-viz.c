#include "dtl-ir-viz.h"

#include <stddef.h>
#include <stdio.h>

#include "dtl-dtype.h"
#include "dtl-ir.h"

static char const *
dtl_ir_viz_get_name(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    if (dtl_ir_is_table_shape_expression(graph, expression)) {
        return "Table (Shape)";
    }
    if (dtl_ir_is_where_shape_expression(graph, expression)) {
        return "Where (Shape)";
    }
    if (dtl_ir_is_join_shape_expression(graph, expression)) {
        return "Join (Shape)";
    }
    if (dtl_ir_is_int64_constant_expression(graph, expression)) {
        return "Int64";
    }
    if (dtl_ir_is_double_constant_expression(graph, expression)) {
        return "Double";
    }
    if (dtl_ir_is_open_table_expression(graph, expression)) {
        return "Open Table";
    }
    if (dtl_ir_is_read_column_expression(graph, expression)) {
        return "Read Column";
    }
    if (dtl_ir_is_where_expression(graph, expression)) {
        return "Where";
    }
    if (dtl_ir_is_pick_expression(graph, expression)) {
        return "Pick";
    }
    if (dtl_ir_is_index_expression(graph, expression)) {
        return "Index";
    }
    if (dtl_ir_is_join_left_expression(graph, expression)) {
        return "Join Left";
    }
    if (dtl_ir_is_join_right_expression(graph, expression)) {
        return "Join Right";
    }
    if (dtl_ir_is_equal_to_expression(graph, expression)) {
        return "Equal To";
    }
    if (dtl_ir_is_less_than_expression(graph, expression)) {
        return "Less Than";
    }
    if (dtl_ir_is_less_than_or_equal_to_expression(graph, expression)) {
        return "Less Than or Equal To";
    }
    if (dtl_ir_is_greater_than_expression(graph, expression)) {
        return "Greater Than";
    }
    if (dtl_ir_is_greater_than_or_equal_to_expression(graph, expression)) {
        return "Greater Than or Equal To";
    }
    if (dtl_ir_is_add_expression(graph, expression)) {
        return "Add";
    }
    if (dtl_ir_is_subtract_expression(graph, expression)) {
        return "Subtract";
    }
    if (dtl_ir_is_multiply_expression(graph, expression)) {
        return "Multiply";
    }
    if (dtl_ir_is_divide_expression(graph, expression)) {
        return "Divide";
    }
    return "Unknown";
}

static char const *
dtl_ir_viz_get_dtype_name(struct dtl_ir_graph *graph, struct dtl_ir_ref expression) {
    switch (dtl_ir_expression_get_dtype(graph, expression)) {
    case DTL_DTYPE_BOOL:
        return "BOOL";
    case DTL_DTYPE_INT64:
        return "INT64";
    case DTL_DTYPE_DOUBLE:
        return "DOUBLE";
    case DTL_DTYPE_STRING:
        return "STRING";
    case DTL_DTYPE_INDEX:
        return "INDEX";
    case DTL_DTYPE_TABLE:
        return "TABLE";

    case DTL_DTYPE_BOOL_ARRAY:
        return "BOOL[]";
    case DTL_DTYPE_INT64_ARRAY:
        return "INT64[]";
    case DTL_DTYPE_DOUBLE_ARRAY:
        return "DOUBLE[]";
    case DTL_DTYPE_STRING_ARRAY:
        return "STRING[]";
    case DTL_DTYPE_INDEX_ARRAY:
        return "INDEX[]";
    }

    return "UNKNOWN";
}

void
dtl_ir_viz(FILE *output, struct dtl_ir_graph *graph) {
    size_t i;
    size_t j;
    size_t size;
    struct dtl_ir_ref ref;
    struct dtl_ir_ref dep;

    fprintf(output, "digraph G {\n");
    fprintf(output, "fontname=\"Helvetica,Arial,sans-serif;\"\n");
    fprintf(output, "concentrate=True;\n");
    fprintf(output, "rankdir=LR;\n");
    fprintf(output, "node [fontname=\"Helvetica,Arial,sans-serif\"];\n");
    fprintf(output, "node [shape=record];\n");
    fprintf(output, "edge [fontname=\"Helvetica,Arial,sans-serif\"]\n");

    size = dtl_ir_graph_get_size(graph);
    for (i = 0; i < size; i++) {
        ref = dtl_ir_index_to_ref(graph, i);

        fprintf(output, "  %li ", i);

        fprintf(output, "[label=\"{");
        for (j = 0; j < dtl_ir_expression_get_num_dependencies(graph, ref); j++) {
            fprintf(output, "%li|", j);
        }
        fprintf(output, "}|%s|%s\"];\n", dtl_ir_viz_get_name(graph, ref), dtl_ir_viz_get_dtype_name(graph, ref));

        for (j = 0; j < dtl_ir_expression_get_num_dependencies(graph, ref); j++) {
            dep = dtl_ir_expression_get_dependency(graph, ref, j);
            fprintf(output, "  %li -> %li;\n", i, dtl_ir_ref_to_index(graph, dep));
        }
    }

    fprintf(output, "}\n");
}
