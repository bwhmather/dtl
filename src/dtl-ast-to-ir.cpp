#include "dtl-ast-to-ir.hpp"

#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <arrow/type.h>

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
column_name_name(dtl::variant_ptr_t<const ColumnName> column_name);
static std::optional<std::string>
column_name_namespace(dtl::variant_ptr_t<const ColumnName> column_name);

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

static std::shared_ptr<const dtl::ir::ArrayExpression>
compile_expression(
    dtl::variant_ptr_t<const Expression> base_expression, std::shared_ptr<Scope> scope,
    Context& context) {
    (void)context;

    if (std::holds_alternative<const ColumnReferenceExpression*>(base_expression)) {
        const auto& expression =
            *std::get<const ColumnReferenceExpression*>(base_expression);

        std::optional<std::string> ns = column_name_namespace(borrow(expression.name));
        std::string name = column_name_name(borrow(expression.name));

        for (auto&& column : scope->columns) {
            if (!column.namespaces.contains(ns)) {
                continue;
            }

            if (column.name != name) {
                continue;
            }

            return column.expression;
        }
        throw "Could not resolve reference";
    }
    if (std::holds_alternative<const LiteralExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const FunctionCallExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const EqualToExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const LessThanExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const LessThanEqualExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const GreaterThanExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const GreaterThanEqualExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const AddExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const SubtractExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const MultiplyExpression*>(base_expression)) {
        throw "Not implemented";
    }
    if (std::holds_alternative<const DivideExpression*>(base_expression)) {
        throw "Not implemented";
    }

    throw "Unreachable";
}

static std::string
column_name_name(dtl::variant_ptr_t<const ColumnName> base_column_name) {
    if (std::holds_alternative<const UnqualifiedColumnName*>(base_column_name)) {
        return std::get<const UnqualifiedColumnName*>(base_column_name)->column_name;
    }

    if (std::holds_alternative<const QualifiedColumnName*>(base_column_name)) {
        return std::get<const QualifiedColumnName*>(base_column_name)->column_name;
    }

    throw "Unreachable";
}

static std::optional<std::string>
column_name_namespace(variant_ptr_t<const ColumnName> base_column_name) {
    if (std::holds_alternative<const UnqualifiedColumnName*>(base_column_name)) {
        return std::optional<std::string>();
    }

    if (std::holds_alternative<const QualifiedColumnName*>(base_column_name)) {
        return std::get<const QualifiedColumnName*>(base_column_name)->table_name;
    }

    throw "Unreachable";
}

static std::string
expression_name(dtl::variant_ptr_t<const Expression> base_expression) {
    if (std::holds_alternative<const ColumnReferenceExpression*>(base_expression)) {
        const auto& expression =
            *std::get<const ColumnReferenceExpression*>(base_expression);
        return column_name_name(borrow(expression.name));
    }

    throw "No name could be derived for expression";
}

static ScopeColumn
compile_column_binding(
    dtl::variant_ptr_t<const ColumnBinding> base_binding, std::shared_ptr<Scope> scope,
    Context& context) {
    if (dtl::get_if<const WildcardColumnBinding*>(&base_binding)) {
        throw "Not implemented";
    }

    if (auto binding = dtl::get_if<const ImplicitColumnBinding*>(&base_binding)) {
        return {
            .name = expression_name(borrow(binding->expression)),
            .namespaces = {{}},
            .expression =
                compile_expression(borrow(binding->expression), scope, context),
        };
    }

    if (auto binding = dtl::get_if<const AliasedColumnBinding*>(&base_binding)) {
        return {
            .name = binding->alias,
            .namespaces = {{}},
            .expression =
                compile_expression(borrow(binding->expression), scope, context),
        };
    }

    throw "Unreachable";
}

static const TableExpression&
table_binding_expression(dtl::variant_ptr_t<const TableBinding> base_binding) {
    if (auto binding = dtl::get_if<const ImplicitTableBinding*>(&base_binding)) {
        return *binding->expression;
    }

    if (auto binding = dtl::get_if<const AliasedTableBinding*>(&base_binding)) {
        return *binding->expression;
    }

    throw "Unreachable";
}

static std::string
table_expression_name(const TableExpression& base_expression) {
    if (base_expression.type() == Type::TABLE_REFERENCE_EXPRESSION) {
        return static_cast<const TableReferenceExpression&>(base_expression)
            .name;
    }

    return "";
}

static std::optional<std::string>
table_binding_name(dtl::variant_ptr_t<const TableBinding> base_binding) {
    if (auto binding = dtl::get_if<const ImplicitTableBinding*>(&base_binding)) {
        return table_expression_name(*binding->expression);
    }

    if (auto binding = dtl::get_if<const AliasedTableBinding*>(&base_binding)) {
        return binding->alias;
    }

    throw "Unreachable";
}

static std::shared_ptr<Scope>
compile_table_expression(
    const TableExpression& base_expression, Context& context) {
    switch (base_expression.type()) {
    case Type::SELECT_EXPRESSION: {
        // TODO
        auto& expression =
            static_cast<const SelectExpression&>(base_expression);
        auto& src_expression =
            table_binding_expression(borrow(expression.source->binding));
        auto src_name = table_binding_name(borrow(expression.source->binding));
        auto src_scope = compile_table_expression(src_expression, context);

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
        for (auto&& column_binding : expression.columns) {
            scope->columns.push_back(
                compile_column_binding(borrow(column_binding), src_scope, context));
        }
        context.trace_table_expression(scope, expression.start, expression.end);

        return scope;
    }
    case Type::IMPORT_EXPRESSION: {
        auto& expression =
            static_cast<const ImportExpression&>(base_expression);
        auto scope = context.import_table(expression.location->value);
        context.trace_table_expression(scope, expression.start, expression.end);
        return scope;
    }

    case Type::TABLE_REFERENCE_EXPRESSION: {
        auto& expression =
            static_cast<const TableReferenceExpression&>(base_expression);
        auto scope = context.get_global(expression.name);
        context.trace_table_expression(scope, expression.start, expression.end);
        return scope;
    }
    default:
        throw "Unreachable";
    }
}

static std::shared_ptr<Scope>
strip_namespaces(std::shared_ptr<Scope> input) {
    auto output = std::make_shared<Scope>();

    for (auto&& input_column : input->columns) {
        assert(input_column.namespaces.contains({}));

        ScopeColumn output_column{
            .name = input_column.name,
            .namespaces = {{}},
            .expression = input_column.expression};

        output->columns.push_back(std::move(output_column));
    }

    return output;
}

static void
compile_statement(const Statement& base_statement, Context& context) {
    switch (base_statement.type()) {
    case Type::ASSIGNMENT_STATEMENT: {
        auto& statement =
            static_cast<const AssignmentStatement&>(base_statement);
        auto expression_table =
            compile_table_expression(*statement.expression, context);

        auto result_table = strip_namespaces(expression_table);

        context.trace_statement(result_table, statement.start, statement.end);
        context.set_global(statement.target->table_name, result_table);
        return;
    }
    case Type::UPDATE_STATEMENT:
        throw "Not implemented";

    case Type::DELETE_STATEMENT:
        throw "Not implemented";

    case Type::INSERT_STATEMENT:
        throw "Not implemented";

    case Type::EXPORT_STATEMENT: {
        auto& statement = static_cast<const ExportStatement&>(base_statement);
        auto expression_table =
            compile_table_expression(*statement.expression, context);

        auto result_table = strip_namespaces(expression_table);

        context.trace_statement(result_table, statement.start, statement.end);
        context.export_table(statement.location->value, result_table);
        return;
    }
    case Type::BEGIN_STATEMENT:
        throw "Not implemented";

    default:
        throw "Unreachable";
    }
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
