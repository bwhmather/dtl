#include "dtl-ast-visit-children.hpp"

#include <functional>
#include <memory>
#include <stddef.h>
#include <string>
#include <vector>

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

static void
visit_string(const String& string, std::function<void(const Node&)> callback) {
    callback(string);
}

static void
visit_literal(
    variant_ptr_t<const Literal> literal, std::function<void(const Node&)> callback) {
    std::visit([&](const String* string) { visit_string(*string, callback); }, literal);
}

static void
visit_unqualified_column_name(
    const UnqualifiedColumnName& column_name,
    std::function<void(const Node&)> callback) {
    callback(column_name);
}

static void
visit_qualified_column_name(
    const QualifiedColumnName& column_name,
    std::function<void(const Node&)> callback) {
    callback(column_name);
}

static void
visit_column_name(
    variant_ptr_t<const ColumnName> base_column_name,
    std::function<void(const Node&)> callback) {
    if (std::holds_alternative<const UnqualifiedColumnName*>(base_column_name)) {
        auto& column_name = std::get<const UnqualifiedColumnName*>(base_column_name);
        visit_unqualified_column_name(*column_name, callback);
    }

    if (std::holds_alternative<const QualifiedColumnName*>(base_column_name)) {
        auto& column_name = std::get<const QualifiedColumnName*>(base_column_name);
        visit_qualified_column_name(*column_name, callback);
    }
}

void
visit_children(const Node& node, std::function<void(const Node&)> callback) {
    callback(node);
    switch (node.type()) {
    // Literals.
    case Type::LITERAL:
        throw "Unreachable";
    case Type::STRING:
        return;

    // Columns.
    case Type::COLUMN_NAME:
        throw "Unreachable";
    case Type::UNQUALIFIED_COLUMN_NAME:
        return;
    case Type::QUALIFIED_COLUMN_NAME:
        return;

    // Expressions.
    case Type::EXPRESSION:
        throw "Unreachable";
    case Type::COLUMN_REFERENCE_EXPRESSION: {
        const auto& expression =
            static_cast<const ColumnReferenceExpression&>(node);
        visit_column_name(borrow(expression.name), callback);
        return;
    }
    case Type::LITERAL_EXPRESSION: {
        const auto& expression = static_cast<const LiteralExpression&>(node);
        visit_literal(borrow(expression.value), callback);
        return;
    }
    case Type::FUNCTION_CALL_EXPRESSION: {
        const auto& expression =
            static_cast<const FunctionCallExpression&>(node);
        for (auto&& argument : expression.arguments) {
            visit_children(*argument, callback);
        }
        return;
    }
    case Type::EQUAL_TO_EXPRESSION: {
        const auto& expression = static_cast<const EqualToExpression&>(node);
        visit_children(*expression.left, callback);
        visit_children(*expression.right, callback);
        return;
    }
    case Type::LESS_THAN_EXPRESSION: {
        const auto& expression = static_cast<const LessThanExpression&>(node);
        visit_children(*expression.left, callback);
        visit_children(*expression.right, callback);
        return;
    }
    case Type::LESS_THAN_EQUAL_EXPRESSION: {
        const auto& expression =
            static_cast<const LessThanEqualExpression&>(node);
        visit_children(*expression.left, callback);
        visit_children(*expression.right, callback);
        return;
    }
    case Type::GREATER_THAN_EXPRESSION: {
        const auto& expression =
            static_cast<const GreaterThanExpression&>(node);
        visit_children(*expression.left, callback);
        visit_children(*expression.right, callback);
        return;
    }
    case Type::GREATER_THAN_EQUAL_EXPRESSION: {
        const auto& expression =
            static_cast<const GreaterThanEqualExpression&>(node);
        visit_children(*expression.left, callback);
        visit_children(*expression.right, callback);
        return;
    }
    case Type::ADD_EXPRESSION: {
        const auto& expression = static_cast<const AddExpression&>(node);
        visit_children(*expression.left, callback);
        visit_children(*expression.right, callback);
        return;
    }
    case Type::SUBTRACT_EXPRESSION: {
        const auto& expression = static_cast<const SubtractExpression&>(node);
        visit_children(*expression.left, callback);
        visit_children(*expression.right, callback);
        return;
    }
    case Type::MULTIPLY_EXPRESSION: {
        const auto& expression = static_cast<const MultiplyExpression&>(node);
        visit_children(*expression.left, callback);
        visit_children(*expression.right, callback);
        return;
    }
    case Type::DIVIDE_EXPRESSION: {
        const auto& expression = static_cast<const DivideExpression&>(node);
        visit_children(*expression.left, callback);
        visit_children(*expression.right, callback);
        return;
    }

    // Tables.
    case Type::TABLE_NAME:
        return;

    // Distinct.
    case Type::DISTINCT_CLAUSE:
        return;

    // Column Bindings.
    case Type::COLUMN_BINDING:
        throw "Unreachable";
    case Type::WILDCARD_COLUMN_BINDING:
        return;
    case Type::IMPLICIT_COLUMN_BINDING: {
        const auto& binding = static_cast<const ImplicitColumnBinding&>(node);
        visit_children(*binding.expression, callback);
        return;
    }
    case Type::ALIASED_COLUMN_BINDING:
        return;

    // From.
    case Type::TABLE_BINDING:
        throw "Unreachable";
    case Type::IMPLICIT_TABLE_BINDING: {
        const auto& binding = static_cast<const ImplicitTableBinding&>(node);
        visit_children(*binding.expression, callback);
        return;
    }
    case Type::ALIASED_TABLE_BINDING: {
        const auto& binding = static_cast<const AliasedTableBinding&>(node);
        visit_children(*binding.expression, callback);
        return;
    }
    case Type::FROM_CLAUSE: {
        const auto& clause = static_cast<const FromClause&>(node);
        visit_children(*clause.binding, callback);
        return;
    }

    // Joins.
    case Type::JOIN_CONSTRAINT:
        throw "Unreachable";
    case Type::JOIN_ON_CONSTRAINT: {
        const auto& constraint = static_cast<const JoinOnConstraint&>(node);
        visit_children(*constraint.predicate, callback);
        return;
    }
    case Type::JOIN_USING_CONSTRAINT: {
        const auto& constraint = static_cast<const JoinUsingConstraint&>(node);
        for (auto&& column : constraint.columns) {
            visit_children(*column, callback);
        }
        return;
    }

    case Type::JOIN_CLAUSE: {
        const auto& clause = static_cast<const JoinClause&>(node);
        visit_children(*clause.binding, callback);
        visit_children(*clause.constraint, callback);
        return;
    }

    // Filtering.
    case Type::WHERE_CLAUSE: {
        const auto& clause = static_cast<const WhereClause&>(node);
        visit_children(*clause.predicate, callback);
        return;
    }

    // Grouping.
    case Type::GROUP_BY_CLAUSE: {
        const auto& clause = static_cast<const GroupByClause&>(node);
        for (auto&& expression : clause.pattern) {
            visit_children(*expression, callback);
        }
        return;
    }

    // Table Expressions.
    case Type::TABLE_EXPRESSION:
        throw "Unreachable";
    case Type::SELECT_EXPRESSION: {
        auto& expression = static_cast<const SelectExpression&>(node);
        if (expression.distinct) {
            visit_children(*expression.distinct, callback);
        }
        for (auto&& column : expression.columns) {
            visit_children(*column, callback);
        }
        visit_children(*expression.source, callback);
        for (auto&& join : expression.joins) {
            visit_children(*join, callback);
        }
        if (expression.where) {
            visit_children(*expression.where, callback);
        }
        if (expression.group_by) {
            visit_children(*expression.group_by, callback);
        }
        return;
    }
    case Type::IMPORT_EXPRESSION: {
        auto& expression = static_cast<const ImportExpression&>(node);
        visit_children(*expression.location, callback);
        return;
    }
    case Type::TABLE_REFERENCE_EXPRESSION:
        return;

    // Statements.
    case Type::STATEMENT:
        throw "Unreachable";
    case Type::ASSIGNMENT_STATEMENT: {
        const auto& statement = static_cast<const AssignmentStatement&>(node);
        visit_children(*statement.target, callback);
        visit_children(*statement.expression, callback);
        return;
    }
    case Type::UPDATE_STATEMENT:
        throw "Not implemented";
    case Type::DELETE_STATEMENT:
        throw "Not implemented";
    case Type::INSERT_STATEMENT:
        throw "Not implemented";
    case Type::EXPORT_STATEMENT: {
        const auto& statement = static_cast<const ExportStatement&>(node);
        visit_children(*statement.location, callback);
        visit_children(*statement.expression, callback);
        return;
    }
    case Type::BEGIN_STATEMENT:
        return;

    // Scripts.
    case Type::SCRIPT: {
        const auto& script = static_cast<const Script&>(node);
        for (auto&& statement : script.statements) {
            visit_children(*statement, callback);
        }
        return;
    }
    }
}

} /* namespace ast */
} /* namespace dtl */
