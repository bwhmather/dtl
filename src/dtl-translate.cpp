#include "dtl-translate.hpp"

#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "dtl-ast.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace translate {

class Context {
    std::unordered_map<std::string, std::shared_ptr<dtl::ir::Table> > m_inputs;
    std::unordered_map<std::string, std::shared_ptr<dtl::ir::Table> > m_globals;

    std::vector<dtl::ir::Trace> m_traces;
    std::vector<dtl::ir::Export> m_exports;

  public:
    void trace_column_expression(
        std::shared_ptr<dtl::ir::Table> table,
        dtl::Location start,
        dtl::Location end
    ) {
        dtl::ir::Trace trace;
        trace.level = dtl::ir::TraceLevel::COLUMN_EXPRESSION;
        trace.table = table;
        trace.start = start;
        trace.end = end;

        m_traces.push_back(std::move(trace));
    }

    void trace_table_expression(
        std::shared_ptr<dtl::ir::Table> table,
        dtl::Location start,
        dtl::Location end
    ) {
        dtl::ir::Trace trace;
        trace.level = dtl::ir::TraceLevel::TABLE_EXPRESSION;
        trace.table = table;
        trace.start = start;
        trace.end = end;

        m_traces.push_back(std::move(trace));
    }

    void trace_statement(
        std::shared_ptr<dtl::ir::Table> table,
        dtl::Location start,
        dtl::Location end
    ) {
        dtl::ir::Trace trace;
        trace.level = dtl::ir::TraceLevel::STATEMENT;
        trace.table = table;
        trace.start = start;
        trace.end = end;

        m_traces.push_back(std::move(trace));
    }

    void trace_assertion(
        std::shared_ptr<dtl::ir::Table> table,
        dtl::Location start,
        dtl::Location end
    ) {
        dtl::ir::Trace trace;
        trace.level = dtl::ir::TraceLevel::ASSERTION;
        trace.table = table;
        trace.start = start;
        trace.end = end;

        m_traces.push_back(std::move(trace));
    }

    std::shared_ptr<dtl::ir::Table> import_table(std::string name) const {
        return m_inputs.at(name);
    }

    void export_table(std::string name, std::shared_ptr<dtl::ir::Table> table) {
        dtl::ir::Export entry;
        entry.name = name;
        entry.table = table;

        m_exports.push_back(std::move(entry));
    }

    void set_global(std::string name, std::shared_ptr<dtl::ir::Table> table) {
        m_globals.insert_or_assign(name, table);
    }

    std::shared_ptr<dtl::ir::Table> get_global(std::string name) const {
        return m_globals.at(name);
    }

    dtl::ir::Program freeze() const {
        dtl::ir::Program program;
        program.traces = m_traces;
        program.exports = m_exports;
        return program;
    }
};

class TableExpressionCompiler : public dtl::ast::TableExpressionVisitor {
    Context m_context;
    std::optional<std::shared_ptr<dtl::ir::Table>> m_result;

  public:
    TableExpressionCompiler(Context context) : m_context(context) {}

    void visit_select_expression(
        dtl::ast::SelectExpression& expr
    ) override final {
        auto table = std::make_shared<dtl::ir::Table>();
        m_context.trace_table_expression(table, expr.start, expr.end);
        m_result = std::move(table);
    };

    void visit_import_expression(
        dtl::ast::ImportExpression& expr
    ) override final {
        auto table = m_context.import_table(expr.location->value);
        m_context.trace_table_expression(table, expr.start, expr.end);
        m_result = std::move(table);
    };

    void visit_table_reference_expression(
        dtl::ast::TableReferenceExpression& expr
    ) override final {
        auto table = m_context.get_global(expr.name);
        m_context.trace_table_expression(table, expr.start, expr.end);
        m_result = std::move(table);
    };

    std::shared_ptr<dtl::ir::Table> visit(dtl::ast::TableExpression& expr) {
        expr.accept(*this);

        std::optional<std::shared_ptr<dtl::ir::Table>> result;
        std::swap(result, m_result);
        return result.value();
    };
};

static std::shared_ptr<dtl::ir::Table>
compile_table_expression(
    dtl::ast::TableExpression& expression, Context context
) {
    TableExpressionCompiler compiler(context);
    return compiler.visit(expression);
}

static std::shared_ptr<dtl::ir::Table> strip_namespaces(std::shared_ptr<dtl::ir::Table> input) {
    auto output = std::make_shared<dtl::ir::Table>();

    for (auto&& input_column : input->columns) {
        assert(input_column.namespaces.contains(""));

        dtl::ir::Column output_column;
        output_column.name = input_column.name;
        output_column.namespaces = {""};
        output_column.expression = input_column.expression;

        output->columns.push_back(std::move(output_column));
    }

    return output;
}

class StatementCompiler : public dtl::ast::StatementVisitor {
    Context m_context;
  public:
    StatementCompiler(Context context) : m_context(context) {};

    void visit_assignment_statement(
        dtl::ast::AssignmentStatement& statement
    ) override final {
        auto expr_table = compile_table_expression(
            *statement.expression, m_context
        );

        auto result_table = strip_namespaces(expr_table);

        m_context.trace_statement(result_table, statement.start, statement.end);
        m_context.set_global(statement.target->table_name, result_table);
    };

    void visit_update_statement(
        dtl::ast::UpdateStatement& statement
    ) override final {
        (void) statement;
        assert(false);
    };

    void visit_delete_statement(
        dtl::ast::DeleteStatement& statement
    ) override final {
        (void) statement;
        assert(false);
    };

    void visit_insert_statement(
        dtl::ast::InsertStatement& statement
    ) override final {
        (void) statement;
        assert(false);
    };

    void visit_export_statement(
        dtl::ast::ExportStatement& statement
    ) override final {
        auto expr_table = compile_table_expression(
            *statement.expression, m_context
        );

        auto result_table = strip_namespaces(expr_table);

        m_context.trace_statement(result_table, statement.start, statement.end);
        m_context.export_table(statement.location->value, result_table);
    };

    void visit(dtl::ast::Statement& statement) {
        statement.accept(*this);
    };
};

static void compile_statement(dtl::ast::Statement& statement, Context context) {
    StatementCompiler compiler(context);
    compiler.visit(statement);
}

dtl::ir::Program ast_to_ir(dtl::ast::Script& script) {
    Context context;

    for (auto&& statement : script.statements) {
        compile_statement(*statement, context);
    }

    return context.freeze();
}

}  /* namespace translate */
}  /* namespace dtl */
