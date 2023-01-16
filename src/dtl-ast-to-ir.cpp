#include "dtl-ast-to-ir.hpp"

#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <arrow/type.h>

#include "dtl-ast-defaulted-visitor.tpp"
#include "dtl-ast-find-imports.hpp"
#include "dtl-ast.hpp"
#include "dtl-io.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace ast {

struct ScopeColumn;
struct Scope;
struct Context;

static std::shared_ptr<Scope>
compile_table_expression(const TableExpression& expression, Context& context);
static std::string
column_name_name(const ColumnName& column_name);
static std::optional<std::string>
column_name_namespace(const ColumnName& column_name);

struct ScopeColumn {
    std::string name;
    std::unordered_set<std::optional<std::string>> namespaces;

    std::shared_ptr<const dtl::ir::ArrayExpression> expression;
};

class Scope {
  public:
    std::vector<ScopeColumn> columns;
};

class Context {
    std::unordered_map<std::string, std::shared_ptr<Scope>> m_inputs;
    std::unordered_map<std::string, std::shared_ptr<Scope>> m_globals;

    std::vector<std::shared_ptr<const dtl::ir::Table>> m_tables;

  public:
    Context(){};
    Context(Context const& context) = delete;
    void
    operator=(Context const&) = delete;

    void
    trace_column_expression(
        std::shared_ptr<Scope> scope, dtl::Location start, dtl::Location end) {
        auto table = std::make_shared<dtl::ir::TraceTable>();

        table->level = dtl::ir::TraceLevel::COLUMN_EXPRESSION;
        table->start = start;
        table->end = end;

        for (auto&& column : scope->columns) {
            table->columns.push_back(std::move(dtl::ir::Column{
                .name = column.name, .expression = column.expression}));
        }

        m_tables.push_back(std::move(table));
    }

    void
    trace_table_expression(
        std::shared_ptr<Scope> scope, dtl::Location start, dtl::Location end) {
        auto table = std::make_shared<dtl::ir::TraceTable>();

        table->level = dtl::ir::TraceLevel::TABLE_EXPRESSION;
        table->start = start;
        table->end = end;

        for (auto&& column : scope->columns) {
            table->columns.push_back(std::move(dtl::ir::Column{
                .name = column.name, .expression = column.expression}));
        }

        m_tables.push_back(std::move(table));
    }

    void
    trace_statement(
        std::shared_ptr<Scope> scope, dtl::Location start, dtl::Location end) {
        auto table = std::make_shared<dtl::ir::TraceTable>();

        table->level = dtl::ir::TraceLevel::STATEMENT;
        table->start = start;
        table->end = end;

        for (auto&& column : scope->columns) {
            table->columns.push_back(std::move(dtl::ir::Column{
                .name = column.name, .expression = column.expression}));
        }

        m_tables.push_back(std::move(table));
    }

    void
    trace_assertion(
        std::shared_ptr<Scope> scope, dtl::Location start, dtl::Location end) {
        auto table = std::make_shared<dtl::ir::TraceTable>();

        table->level = dtl::ir::TraceLevel::ASSERTION;
        table->start = start;
        table->end = end;

        for (auto&& column : scope->columns) {
            table->columns.push_back(std::move(dtl::ir::Column{
                .name = column.name, .expression = column.expression}));
        }

        m_tables.push_back(std::move(table));
    }

    void
    add_input(std::string name, std::shared_ptr<Scope> scope) {
        m_inputs.insert_or_assign(name, scope);
    }

    std::shared_ptr<Scope>
    import_table(std::string name) const {
        return m_inputs.at(name);
    }

    void
    export_table(std::string name, std::shared_ptr<Scope> scope) {
        auto table = std::make_shared<dtl::ir::ExportTable>();

        table->name = name;

        for (auto&& column : scope->columns) {
            table->columns.push_back(std::move(dtl::ir::Column{
                .name = column.name, .expression = column.expression}));
        }

        m_tables.push_back(std::move(table));
    }

    void
    set_global(std::string name, std::shared_ptr<Scope> scope) {
        m_globals.insert_or_assign(name, scope);
    }

    std::shared_ptr<Scope>
    get_global(std::string name) const {
        return m_globals.at(name);
    }

    std::vector<std::shared_ptr<const dtl::ir::Table>>
    freeze() {
        return std::move(m_tables);
    }
};

class ExpressionCompiler : public ExpressionVisitor {
    std::shared_ptr<Scope> m_scope;
    Context& m_context;

    std::optional<std::shared_ptr<const dtl::ir::ArrayExpression>> m_result;

  public:
    ExpressionCompiler(std::shared_ptr<Scope> scope, Context& context) :
        m_scope(scope), m_context(context) {}

    void
    visit_column_reference_expression(
        const ColumnReferenceExpression& expression) override {
        std::optional<std::string> ns = column_name_namespace(*expression.name);
        std::string name = column_name_name(*expression.name);

        for (auto&& column : m_scope->columns) {
            if (!column.namespaces.contains(ns)) {
                continue;
            }

            if (column.name != name) {
                continue;
            }

            m_result = column.expression;
            return;
        }
        throw "Could not resolve reference";
    }

    void
    visit_literal_expression(const LiteralExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_function_call_expression(
        const FunctionCallExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_equal_to_expression(const EqualToExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_less_than_expression(const LessThanExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_less_than_equal_expression(
        const LessThanEqualExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_greater_than_expression(
        const GreaterThanExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_greater_than_equal_expression(
        const GreaterThanEqualExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_add_expression(const AddExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_subtract_expression(const SubtractExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_multiply_expression(const MultiplyExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    void
    visit_divide_expression(const DivideExpression& expression) override {
        (void)expression;
        throw "Not implemented";
    }

    std::shared_ptr<const dtl::ir::ArrayExpression>
    run(const Expression& expression) {
        expression.accept(*this);
        return std::move(m_result.value());
    }
};

static std::shared_ptr<const dtl::ir::ArrayExpression>
compile_expression(
    const Expression& expression, std::shared_ptr<Scope> scope,
    Context& context) {
    ExpressionCompiler compiler(scope, context);
    return compiler.run(expression);
}

class ColumnNameNameVisitor : public ColumnNameVisitor {
    std::optional<std::string> m_result;

  public:
    void
    visit_unqualified_column_name(
        const UnqualifiedColumnName& column_name) override {
        m_result = column_name.column_name;
    }

    void
    visit_qualified_column_name(
        const QualifiedColumnName& column_name) override {
        m_result = column_name.column_name;
    }

    std::string
    run(const ColumnName& column_name) {
        column_name.accept(*this);
        return std::move(m_result.value());
    }
};

static std::string
column_name_name(const ColumnName& column_name) {
    ColumnNameNameVisitor visitor;
    return visitor.run(column_name);
}

class ColumnNameNamespaceVisitor : public ColumnNameVisitor {
    std::optional<std::optional<std::string>> m_result;

  public:
    void
    visit_unqualified_column_name(const UnqualifiedColumnName&) override {
        m_result.emplace();
    }

    void
    visit_qualified_column_name(
        const QualifiedColumnName& column_name) override {
        m_result = column_name.table_name;
    }

    std::optional<std::string>
    run(const ColumnName& column_name) {
        column_name.accept(*this);
        return std::move(m_result.value());
    }
};

static std::optional<std::string>
column_name_namespace(const ColumnName& column_name) {
    ColumnNameNamespaceVisitor visitor;
    return visitor.run(column_name);
}

class ExpressionNameVisitor :
    public DefaultedExpressionVisitorMixin<ExpressionVisitor> {
    std::optional<std::string> m_result;

  public:
    void
    visit_column_reference_expression(
        const ColumnReferenceExpression& expression) override {
        m_result = column_name_name(*expression.name);
    }

    void
    visit_expression(const Expression& expression) override {
        (void)expression;
        throw "No name could be derived for expression";
    }

    std::string
    run(const Expression& expression) {
        expression.accept(*this);
        return std::move(m_result.value());
    };
};

static std::string
expression_name(const Expression& expression) {
    ExpressionNameVisitor visitor;
    return visitor.run(expression);
}

class ColumnBindingCompiler : public ColumnBindingVisitor {
    std::shared_ptr<Scope> m_scope;
    Context& m_context;

    std::optional<ScopeColumn> m_result;

  public:
    ColumnBindingCompiler(std::shared_ptr<Scope> scope, Context& context) :
        m_scope(scope), m_context(context) {}

    void
    visit_wildcard_column_binding(
        const WildcardColumnBinding& binding) override {
        (void)binding;
        throw "Not implemented";
    };

    void
    visit_implicit_column_binding(
        const ImplicitColumnBinding& binding) override {
        m_result = {
            .name = expression_name(*binding.expression),
            .namespaces = {{}},
            .expression =
                compile_expression(*binding.expression, m_scope, m_context)};
    }

    void
    visit_aliased_column_binding(const AliasedColumnBinding& binding) override {
        m_result = {
            .name = binding.alias,
            .namespaces = {{}},
            .expression =
                compile_expression(*binding.expression, m_scope, m_context)};
    }

    ScopeColumn
    run(const ColumnBinding& binding) {
        binding.accept(*this);
        return std::move(m_result.value());
    }
};

static ScopeColumn
compile_column_binding(
    const ColumnBinding& binding, std::shared_ptr<Scope> scope,
    Context& context) {
    ColumnBindingCompiler compiler(scope, context);
    return compiler.run(binding);
}

class TableBindingExpressionVisitor : public TableBindingVisitor {
    std::optional<const TableExpression*> m_result;

  public:
    void
    visit_implicit_table_binding(const ImplicitTableBinding& binding) override {
        m_result = binding.expression.get();
    }

    void
    visit_aliased_table_binding(const AliasedTableBinding& binding) override {
        m_result = binding.expression.get();
    }

    const TableExpression&
    run(const TableBinding& binding) {
        binding.accept(*this);
        return *std::move(m_result.value());
    };
};

static const TableExpression&
table_binding_expression(const TableBinding& binding) {
    TableBindingExpressionVisitor visitor;
    return visitor.run(binding);
}

class TableExpressionNameVisitor :
    public DefaultedTableExpressionVisitorMixin<TableExpressionVisitor> {
    std::optional<std::string> m_result;

  public:
    void
    visit_table_expression(const TableExpression&) override {
        m_result = std::string();
    }

    void
    visit_table_reference_expression(
        const TableReferenceExpression& expression) override {
        m_result = expression.name;
    }

    std::string
    run(const TableExpression& expression) {
        expression.accept(*this);
        return std::move(m_result.value());
    };
};

static std::string
table_expression_name(const TableExpression& expression) {
    TableExpressionNameVisitor visitor;
    return visitor.run(expression);
}

class TableBindingNameVisitor : public TableBindingVisitor {
    std::optional<std::optional<std::string>> m_result;

  public:
    void
    visit_implicit_table_binding(const ImplicitTableBinding& binding) override {
        m_result = table_expression_name(*binding.expression);
    }

    void
    visit_aliased_table_binding(const AliasedTableBinding& binding) override {
        m_result = binding.alias;
    }

    std::optional<std::string>
    run(const TableBinding& binding) {
        binding.accept(*this);
        return std::move(m_result.value());
    };
};

static std::optional<std::string>
table_binding_name(const TableBinding& binding) {
    TableBindingNameVisitor visitor;
    return visitor.run(binding);
}

class TableExpressionCompiler : public TableExpressionVisitor {
    Context& m_context;
    std::optional<std::shared_ptr<Scope>> m_result;

  public:
    TableExpressionCompiler(Context& context) : m_context(context) {}

    void
    visit_select_expression(const SelectExpression& expression) override final {
        // TODO
        auto& src_expression =
            table_binding_expression(*expression.source->binding);
        auto src_name = table_binding_name(*expression.source->binding);
        auto src_scope = compile_table_expression(src_expression, m_context);

        std::vector<ScopeColumn> src_columns;
        for (auto& src_column : src_scope->columns) {
            std::unordered_set<std::optional<std::string>> namespaces(
                src_column.namespaces);
            namespaces.insert({}); // TODO don't do this if binding is aliased.
            namespaces.insert(src_name);

            ScopeColumn column{
                .name = src_column.name,
                .namespaces = std::move(namespaces),
                .expression = src_column.expression};
            src_columns.push_back(std::move(column));
        }

        // Not traced because the same expression should already have been
        // traced above in the call to `compile_table_expression`.
        src_scope =
            std::make_shared<Scope>(Scope{.columns = std::move(src_columns)});

        // TODO join clauses.

        // TODO where clauses.

        // TODO group by.

        // TODO wildcards column bindings.
        // TODO duplicate column bindings.

        auto scope = std::make_shared<Scope>();
        for (auto&& column_binding_ptr : expression.columns) {
            auto& column_binding = *column_binding_ptr;

            scope->columns.push_back(
                compile_column_binding(column_binding, src_scope, m_context));
        }
        m_context.trace_table_expression(
            scope, expression.start, expression.end);

        m_result = std::move(scope);
    };

    void
    visit_import_expression(const ImportExpression& expression) override final {
        auto scope = m_context.import_table(expression.location->value);
        m_context.trace_table_expression(
            scope, expression.start, expression.end);
        m_result = std::move(scope);
    };

    void
    visit_table_reference_expression(
        const TableReferenceExpression& expression) override final {
        auto scope = m_context.get_global(expression.name);
        m_context.trace_table_expression(
            scope, expression.start, expression.end);
        m_result = std::move(scope);
    };

    std::shared_ptr<Scope>
    run(const TableExpression& expression) {
        expression.accept(*this);

        std::optional<std::shared_ptr<Scope>> result;
        std::swap(result, m_result);
        return result.value();
    };
};

static std::shared_ptr<Scope>
compile_table_expression(const TableExpression& expression, Context& context) {
    TableExpressionCompiler compiler(context);
    return compiler.run(expression);
}

static std::shared_ptr<Scope>
strip_namespaces(std::shared_ptr<Scope> input) {
    auto output = std::make_shared<Scope>();

    for (auto&& input_column : input->columns) {
        assert(input_column.namespaces.contains({}));

        ScopeColumn output_column {
            .name = input_column.name,
            .namespaces = {{}},
            .expression = input_column.expression};

        output->columns.push_back(std::move(output_column));
    }

    return output;
}

class StatementCompiler : public StatementVisitor {
    Context& m_context;

  public:
    StatementCompiler(Context& context) : m_context(context){};

    void
    visit_assignment_statement(
        const AssignmentStatement& statement) override final {
        auto expression_table =
            compile_table_expression(*statement.expression, m_context);

        auto result_table = strip_namespaces(expression_table);

        m_context.trace_statement(result_table, statement.start, statement.end);
        m_context.set_global(statement.target->table_name, result_table);
    };

    void
    visit_update_statement(const UpdateStatement& statement) override final {
        (void)statement;
        assert(false);
    };

    void
    visit_delete_statement(const DeleteStatement& statement) override final {
        (void)statement;
        assert(false);
    };

    void
    visit_insert_statement(const InsertStatement& statement) override final {
        (void)statement;
        assert(false);
    };

    void
    visit_export_statement(const ExportStatement& statement) override final {
        auto expression_table =
            compile_table_expression(*statement.expression, m_context);

        auto result_table = strip_namespaces(expression_table);

        m_context.trace_statement(result_table, statement.start, statement.end);
        m_context.export_table(statement.location->value, result_table);
    };

    void
    visit_begin_statement(const BeginStatement& statement) override final {
        (void)statement;
        assert(false);
    }
};

static void
compile_statement(const Statement& statement, Context& context) {
    StatementCompiler compiler(context);
    statement.accept(compiler);
}

static void
compile_input_table(
    const std::string& table_name, std::shared_ptr<arrow::Schema> schema,
    Context& context) {
    auto scope = std::make_shared<Scope>();

    for (int i = 0; i < schema->num_fields(); i++) {
        auto field = schema->field(i);

        auto expression = std::make_shared<dtl::ir::ImportExpression>();
        expression->dtype = dtl::ir::DType::DOUBLE; // TODO field->type->type
        expression->location = table_name;
        expression->name = field->name();

        ScopeColumn column{
            .name = field->name(),
            .namespaces = {{}},
            .expression = expression};
        scope->columns.push_back(std::move(column));
    }

    context.add_input(table_name, scope);
}

std::vector<std::shared_ptr<const dtl::ir::Table>>
to_ir(Script& script, dtl::io::Importer& importer) {
    Context context;

    std::vector<std::string> import_names = find_imports(script);
    for (auto&& name : import_names) {
        auto schema = importer.import_schema(name);
        compile_input_table(name, schema, context);
    }

    for (auto&& statement : script.statements) {
        compile_statement(*statement, context);
    }

    return context.freeze();
}

} /* namespace ast */
} /* namespace dtl */
