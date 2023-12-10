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

static void
visit_expression(
    variant_ptr_t<const Expression> base_expression,
    std::function<void(const Node&)> callback);

static void
visit_column_reference_expression(const ColumnReferenceExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_column_name(borrow(expression.name), callback);
}

static void
visit_literal_expression(const LiteralExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_literal(borrow(expression.value), callback);
}

static void
visit_function_call_expression(const FunctionCallExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    for (auto&& argument : expression.arguments) {
        visit_expression(borrow(argument), callback);
    }
}

static void
visit_equal_to_expression(const EqualToExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_expression(borrow(expression.left), callback);
    visit_expression(borrow(expression.right), callback);
}

static void
visit_less_than_expression(const LessThanExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_expression(borrow(expression.left), callback);
    visit_expression(borrow(expression.right), callback);
}

static void
visit_less_than_equal_expression(const LessThanEqualExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_expression(borrow(expression.left), callback);
    visit_expression(borrow(expression.right), callback);
}

static void
visit_greater_than_expression(const GreaterThanExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_expression(borrow(expression.left), callback);
    visit_expression(borrow(expression.right), callback);
}

static void
visit_greater_than_equal_expression(const GreaterThanEqualExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_expression(borrow(expression.left), callback);
    visit_expression(borrow(expression.right), callback);
}

static void
visit_add_expression(const AddExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_expression(borrow(expression.left), callback);
    visit_expression(borrow(expression.right), callback);
}

static void
visit_subtract_expression(const SubtractExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_expression(borrow(expression.left), callback);
    visit_expression(borrow(expression.right), callback);
}

static void
visit_multiply_expression(const MultiplyExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_expression(borrow(expression.left), callback);
    visit_expression(borrow(expression.right), callback);
}

static void
visit_divide_expression(const DivideExpression& expression, std::function<void(const Node&)> callback) {
    callback(expression);
    visit_expression(borrow(expression.left), callback);
    visit_expression(borrow(expression.right), callback);
}

static void
visit_expression(
    variant_ptr_t<const Expression> base_expression,
    std::function<void(const Node&)> callback) {
    if (auto expression = dtl::get_if<const ColumnReferenceExpression*>(&base_expression)) {
        visit_column_reference_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const LiteralExpression*>(&base_expression)) {
        visit_literal_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const FunctionCallExpression*>(&base_expression)) {
        visit_function_call_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const EqualToExpression*>(&base_expression)) {
        visit_equal_to_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const LessThanExpression*>(&base_expression)) {
        visit_less_than_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const LessThanEqualExpression*>(&base_expression)) {
        visit_less_than_equal_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const GreaterThanExpression*>(&base_expression)) {
        visit_greater_than_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const GreaterThanEqualExpression*>(&base_expression)) {
        visit_greater_than_equal_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const AddExpression*>(&base_expression)) {
        visit_add_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const SubtractExpression*>(&base_expression)) {
        visit_subtract_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const MultiplyExpression*>(&base_expression)) {
        visit_multiply_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const DivideExpression*>(&base_expression)) {
        visit_divide_expression(*expression, callback);
    };
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
    case Type::COLUMN_REFERENCE_EXPRESSION:
        throw "Unreachable";
    case Type::LITERAL_EXPRESSION:
        throw "Unreachable";
    case Type::FUNCTION_CALL_EXPRESSION:
        throw "Unreachable";
    case Type::EQUAL_TO_EXPRESSION:
        throw "Unreachable";
    case Type::LESS_THAN_EXPRESSION:
        throw "Unreachable";
    case Type::LESS_THAN_EQUAL_EXPRESSION:
        throw "Unreachable";
    case Type::GREATER_THAN_EXPRESSION:
        throw "Unreachable";
    case Type::GREATER_THAN_EQUAL_EXPRESSION:
        throw "Unreachable";
    case Type::ADD_EXPRESSION:
        throw "Unreachable";
    case Type::SUBTRACT_EXPRESSION:
        throw "Unreachable";
    case Type::MULTIPLY_EXPRESSION:
        throw "Unreachable";
    case Type::DIVIDE_EXPRESSION:
        throw "Unreachable";

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
        visit_expression(borrow(binding.expression), callback);
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
        visit_expression(borrow(constraint.predicate), callback);
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
        visit_expression(borrow(clause.predicate), callback);
        return;
    }

    // Grouping.
    case Type::GROUP_BY_CLAUSE: {
        const auto& clause = static_cast<const GroupByClause&>(node);
        for (auto&& expression : clause.pattern) {
            visit_expression(borrow(expression), callback);
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
