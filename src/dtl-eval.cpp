#include "dtl-eval.hpp"

#include <memory>

#include "dtl-ast-to-ir.hpp"
#include "dtl-ast.hpp"
#include "dtl-cmd.hpp"
#include "dtl-io.hpp"
#include "dtl-ir-to-cmd.hpp"
#include "dtl-ir.hpp"
#include "dtl-parser.hpp"
#include "dtl-tokenizer.hpp"

namespace dtl {
namespace eval {

class ExtractExportTablesVisitor : public dtl::ir::TableVisitor {
    std::vector<std::shared_ptr<const dtl::ir::ExportTable>> m_export_tables;

  public:
    void
    visit_trace_table(const dtl::ir::TraceTable& table) override final {
        // TODO
        (void)table;
    };

    void
    visit_export_table(const dtl::ir::ExportTable& table) override final {
        m_export_tables.push_back(
            std::static_pointer_cast<const dtl::ir::ExportTable>(
                table.shared_from_this()));
    };

    std::vector<std::shared_ptr<const dtl::ir::ExportTable>>
    result(void) {
        return std::move(m_export_tables);
    }
};

static std::vector<std::shared_ptr<const dtl::ir::ExportTable>>
extract_export_tables(
    std::vector<std::shared_ptr<const dtl::ir::Table>> tables) {
    ExtractExportTablesVisitor visitor;
    for (auto&& table : tables) {
        table->accept(visitor);
    }

    return std::move(visitor.result());
}

struct EvalContext {
    std::unordered_map<std::shared_ptr<const dtl::ir::ShapeExpression>, int>
        shapes;
    std::unordered_map<
        std::shared_ptr<const dtl::ir::ArrayExpression>,
        std::shared_ptr<arrow::ChunkedArray>>
        arrays;
    dtl::io::Importer& importer;
    dtl::io::Exporter& exporter;
    dtl::io::Tracer& tracer;
};

class EvalShapeExpressionVisitor : public dtl::ir::ShapeExpressionVisitor {
    EvalContext& m_context;

  public:
    EvalShapeExpressionVisitor(EvalContext& context) : m_context(context){};

    void
    visit_import_shape_expression(
        const dtl::ir::ImportShapeExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_join_shape_expression(
        const dtl::ir::JoinShapeExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_where_shape_expression(
        const dtl::ir::WhereShapeExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }
};

void
eval_shape_expression(
    EvalContext& context, const dtl::ir::ShapeExpression& expression) {
    EvalShapeExpressionVisitor visitor(context);
    expression.accept(visitor);
}

class EvalArrayExpressionVisitor : public dtl::ir::ArrayExpressionVisitor {
    EvalContext& m_context;

  public:
    EvalArrayExpressionVisitor(EvalContext& context) : m_context(context){};

    void
    visit_import_expression(
        const dtl::ir::ImportExpression& expression) override final {
        auto table = m_context.importer.import_table(expression.location);
        auto array = table->GetColumnByName(expression.name);
        m_context.arrays[expression.get_ptr()] = array;
    }

    void
    visit_where_expression(
        const dtl::ir::WhereExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_pick_expression(
        const dtl::ir::PickExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_index_expression(
        const dtl::ir::IndexExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_join_left_expression(
        const dtl::ir::JoinLeftExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_join_right_expression(
        const dtl::ir::JoinRightExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_add_expression(
        const dtl::ir::AddExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_subtract_expression(
        const dtl::ir::SubtractExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_multiply_expression(
        const dtl::ir::MultiplyExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_divide_expression(
        const dtl::ir::DivideExpression& expression) override final {
        // TODO
        (void)expression;
        throw "Not implemented";
    }
};

void
eval_array_expression(
    EvalContext& context, const dtl::ir::ArrayExpression& expression) {
    EvalArrayExpressionVisitor visitor(context);
    expression.accept(visitor);
}

class EvalCommandVisitor : public dtl::cmd::CommandVisitor {
    EvalContext& m_context;

  public:
    EvalCommandVisitor(EvalContext& context) : m_context(context){};

    void
    visit_evaluate_array_command(
        const dtl::cmd::EvaluateArrayCommand& cmd) override final {
        eval_array_expression(m_context, *cmd.expression);
    }

    void
    visit_evaluate_shape_command(
        const dtl::cmd::EvaluateShapeCommand& cmd) override final {
        // TODO
        (void)cmd;
        throw "Not implemented";
    }

    void
    visit_collect_array_command(
        const dtl::cmd::CollectArrayCommand& cmd) override final {
        // TODO
        (void)cmd;
        throw "Not implemented";
    }

    void
    visit_trace_array_command(
        const dtl::cmd::TraceArrayCommand& cmd) override final {
        // TODO
        (void)cmd;
        throw "Not implemented";
    }

    void
    visit_export_table_command(
        const dtl::cmd::ExportTableCommand& cmd) override final {
        const dtl::ir::ExportTable& description = *cmd.table;

        std::vector<std::shared_ptr<arrow::Field>> fields;
        std::vector<std::shared_ptr<arrow::ChunkedArray>> arrays;
        for (auto&& column : description.columns) {
            auto array = m_context.arrays.at(column.expression);
            auto field = arrow::field(column.name, array->type());

            fields.push_back(field);
            arrays.push_back(array);
        }
        auto schema = arrow::schema(fields);
        auto table = arrow::Table::Make(schema, arrays);

        m_context.exporter.export_table(description.name, table);
    }
};

void
eval_command(EvalContext& context, const dtl::cmd::Command& command) {
    EvalCommandVisitor visitor(context);
    command.accept(visitor);
}

void
run(std::string source, dtl::io::Importer& importer,
    dtl::io::Exporter& exporter, dtl::io::Tracer& tracer) {
    // === Parse Source Code ===================================================
    dtl::tokenizer::Tokenizer tokenizer(source.begin(), source.end());
    auto ast = dtl::parser::parse(tokenizer);

    // === Compile AST to list of tables referencing IR expressions ============
    auto tables = dtl::ast::to_ir(*ast, importer);
    auto export_tables = extract_export_tables(tables);

    /*
    auto trace_tables = dtl::ir::extract_trace_tables(
        program.tables, level=ir.Level.COLUMN_EXPRESSION
    )
    auto export_tables = dtl::ir::extract_export_tables(program.tables)
    // all_tables = list(set(*trace_tables, *export_tables))
    */

    // === Optimise IR =========================================================
    // Optimise joins.
    // TODO

    // Deduplicate IR expressions.
    // TODO.

    // After this point, the expression graph is frozen.  We no longer need to
    // update mappings.

    // === Generate Mappings ===================================================
    // Generate initial mappings for all reachable expression pairs.
    // TODO

    // Merge mappings between expressions that aren't in the roots list.
    // TODO

    /* mappings: list[Mapping] = []*/

    // === Compile Reachable Expressions to Command List =======================
    // Find reachable expressions.
    std::vector<std::shared_ptr<const dtl::ir::Expression>> roots;
    for (auto&& table : tables) {
        for (auto&& column : table->columns) {
            if (std::find(
                    std::begin(roots), std::end(roots), column.expression) ==
                std::end(roots)) {
                roots.push_back(column.expression);
            }
        }
    }

    // TODO find reachable expressions in trace tables and mappings.

    // Compile to command list.
    auto commands = dtl::ir::to_cmd(roots);

    // === Inject Commands to Export Tables ====================================
    for (auto&& table : export_tables) {
        commands.push_back(
            std::make_unique<dtl::cmd::ExportTableCommand>(table));
    }

    // === Setup Tracing =======================================================
    /*
    if tracer is not None:
        // Generate identifiers for arrays referenced by trace tables and
        // mappings and inject commands to export them.
        names: dict[ir.Expression, UUID] = {}
        for expression in get_table_roots(trace_tables) + get_mapping_roots(
            mappings
        ):
            assert isinstance(expression, ir.ArrayExpression)

            names[expression] = uuid4()
            // TODO these commands should be injected immediately after where
            // the array is defined.
            commands.append(
                cmd.TraceArrayCommand(
                    expression=expression,
                    uuid=names[expression],
                )
            )

        // Write trace manifest.
        manifest = create_manifest(
            source=source,
            snapshots=trace_tables,
            mappings=mappings,
            names=names,
        )

        tracer.write_manifest(manifest)
    */

    // === Inject Commands to Collect Arrays After Use =========================
    // TODO

    // === Evaluate the command list ==========================================
    auto context = EvalContext{
        .shapes = {},
        .arrays = {},
        .importer = importer,
        .exporter = exporter,
        .tracer = tracer,
    };

    for (auto&& command : commands) {
        eval_command(context, *command);
    }
}

} /* namespace eval */
} /* namespace dtl */
