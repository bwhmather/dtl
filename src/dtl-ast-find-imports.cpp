#include "dtl-ast-find-imports.hpp"

#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "dtl-ast.hpp"
#include "dtl-variant.tpp"

namespace dtl {
namespace ast {

void
find_imports_in_table_expression(
    dtl::variant_ptr<const TableExpression> base_expression,
    std::function<void(const std::string&)> callback
);

void
find_imports_in_expression(
    variant_ptr<const Expression> base_expression,
    std::function<void(const std::string&)> callback
);

void
find_imports_in_function_call_expression(
    const FunctionCallExpression& expression,
    std::function<void(const std::string&)> callback
) {
    for (auto&& argument : expression.arguments) {
        find_imports_in_expression(borrow(argument), callback);
    }
}

void
find_imports_in_equal_to_expression(
    const EqualToExpression& expression,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(expression.left), callback);
    find_imports_in_expression(borrow(expression.right), callback);
}

void
find_imports_in_less_than_expression(
    const LessThanExpression& expression,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(expression.left), callback);
    find_imports_in_expression(borrow(expression.right), callback);
}

void
find_imports_in_less_than_equal_expression(
    const LessThanOrEqualExpression& expression,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(expression.left), callback);
    find_imports_in_expression(borrow(expression.right), callback);
}

void
find_imports_in_greater_than_expression(
    const GreaterThanExpression& expression,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(expression.left), callback);
    find_imports_in_expression(borrow(expression.right), callback);
}

void
find_imports_in_greater_than_equal_expression(
    const GreaterThanOrEqualExpression& expression,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(expression.left), callback);
    find_imports_in_expression(borrow(expression.right), callback);
}

void
find_imports_in_add_expression(
    const AddExpression& expression,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(expression.left), callback);
    find_imports_in_expression(borrow(expression.right), callback);
}

void
find_imports_in_subtract_expression(
    const SubtractExpression& expression,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(expression.left), callback);
    find_imports_in_expression(borrow(expression.right), callback);
}

void
find_imports_in_multiply_expression(
    const MultiplyExpression& expression,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(expression.left), callback);
    find_imports_in_expression(borrow(expression.right), callback);
}

void
find_imports_in_divide_expression(
    const DivideExpression& expression,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(expression.left), callback);
    find_imports_in_expression(borrow(expression.right), callback);
}

void
find_imports_in_expression(
    variant_ptr<const Expression> base_expression,
    std::function<void(const std::string&)> callback
) {
    if (auto expression = dtl::get_if<const FunctionCallExpression*>(base_expression)) {
        find_imports_in_function_call_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const EqualToExpression*>(base_expression)) {
        find_imports_in_equal_to_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const LessThanExpression*>(base_expression)) {
        find_imports_in_less_than_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const LessThanOrEqualExpression*>(base_expression)) {
        find_imports_in_less_than_equal_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const GreaterThanExpression*>(base_expression)) {
        find_imports_in_greater_than_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const GreaterThanOrEqualExpression*>(base_expression)) {
        find_imports_in_greater_than_equal_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const AddExpression*>(base_expression)) {
        find_imports_in_add_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const SubtractExpression*>(base_expression)) {
        find_imports_in_subtract_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const MultiplyExpression*>(base_expression)) {
        find_imports_in_multiply_expression(*expression, callback);
    };
    if (auto expression = dtl::get_if<const DivideExpression*>(base_expression)) {
        find_imports_in_divide_expression(*expression, callback);
    };
}

void
find_imports_in_implicit_column_binding(
    const ImplicitColumnBinding& binding,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(binding.expression), callback);
}

void
find_imports_in_aliased_column_binding(
    const AliasedColumnBinding& binding,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(binding.expression), callback);
}

void
find_imports_in_column_binding(
    variant_ptr<const ColumnBinding> base_binding,
    std::function<void(const std::string&)> callback
) {
    if (auto binding = dtl::get_if<const ImplicitColumnBinding*>(base_binding)) {
        find_imports_in_implicit_column_binding(*binding, callback);
    };
    if (auto binding = dtl::get_if<const AliasedColumnBinding*>(base_binding)) {
        find_imports_in_aliased_column_binding(*binding, callback);
    };
}

void
find_imports_in_implicit_table_binding(
    const ImplicitTableBinding& binding,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_table_expression(borrow(binding.expression), callback);
}

void
find_imports_in_aliased_table_binding(
    const AliasedTableBinding& binding,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_table_expression(borrow(binding.expression), callback);
}

void
find_imports_in_table_binding(
    variant_ptr<const TableBinding> base_binding,
    std::function<void(const std::string&)> callback
) {
    if (auto binding = dtl::get_if<const ImplicitTableBinding*>(base_binding)) {
        find_imports_in_implicit_table_binding(*binding, callback);
    };
    if (auto binding = dtl::get_if<const AliasedTableBinding*>(base_binding)) {
        find_imports_in_aliased_table_binding(*binding, callback);
    };
}

void
find_imports_in_from_clause(
    const FromClause& clause,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_table_binding(borrow(clause.binding), callback);
}

void
find_imports_in_join_on_constraint(
    const JoinOnConstraint& constraint,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(constraint.predicate), callback);
}

void
find_imports_in_join_constraint(
    variant_ptr<const JoinConstraint> base_constraint,
    std::function<void(const std::string&)> callback
) {
    if (auto constraint = dtl::get_if<const JoinOnConstraint*>(base_constraint)) {
        find_imports_in_join_on_constraint(*constraint, callback);
    };
}

void
find_imports_in_join_clause(
    const JoinClause& clause,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_table_binding(borrow(clause.binding), callback);
    find_imports_in_join_constraint(borrow(clause.constraint), callback);
}

void
find_imports_in_where_clause(
    const WhereClause& clause,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_expression(borrow(clause.predicate), callback);
}

void
find_imports_in_group_by_clause(
    const GroupByClause& clause,
    std::function<void(const std::string&)> callback
) {
    for (auto&& expression : clause.pattern) {
        find_imports_in_expression(borrow(expression), callback);
    }
}

void
find_imports_in_select_expression(
    const SelectExpression& expression,
    std::function<void(const std::string&)> callback
) {
    for (auto&& column : expression.columns) {
        find_imports_in_column_binding(borrow(column), callback);
    }
    find_imports_in_from_clause(*expression.source, callback);
    for (auto&& join : expression.joins) {
        find_imports_in_join_clause(*join, callback);
    }
    if (expression.where) {
        find_imports_in_where_clause(*expression.where, callback);
    }
    if (expression.group_by) {
        find_imports_in_group_by_clause(*expression.group_by, callback);
    }
}

void
find_imports_in_import_expression(
    const ImportExpression& expression,
    std::function<void(const std::string&)> callback
) {
    callback(expression.location->value);
}

void
find_imports_in_table_expression(
    dtl::variant_ptr<const TableExpression> base_expression,
    std::function<void(const std::string&)> callback
) {
    if (auto expression = dtl::get_if<const SelectExpression*>(base_expression)) {
        find_imports_in_select_expression(*expression, callback);
        return;
    }

    if (auto expression = dtl::get_if<const ImportExpression*>(base_expression)) {
        find_imports_in_import_expression(*expression, callback);
        return;
    }
}

void
find_imports_in_assignment_statement(
    const AssignmentStatement& statement,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_table_expression(borrow(statement.expression), callback);
}

void
find_imports_in_update_statement(
    const UpdateStatement& statement,
    std::function<void(const std::string&)> callback
) {
    (void)statement;
    (void)callback;
    throw std::logic_error("Not implement");
}

void
find_imports_in_delete_statement(
    const DeleteStatement& statement,
    std::function<void(const std::string&)> callback
) {
    (void)statement;
    (void)callback;
    throw std::logic_error("Not implement");
}

void
find_imports_in_insert_statement(
    const InsertStatement& statement,
    std::function<void(const std::string&)> callback
) {
    (void)statement;
    (void)callback;
    throw std::logic_error("Not implement");
}

void
find_imports_in_export_statement(
    const ExportStatement& statement,
    std::function<void(const std::string&)> callback
) {
    find_imports_in_table_expression(borrow(statement.expression), callback);
}

void
find_imports_in_begin_statement(
    const BeginStatement& statement,
    std::function<void(const std::string&)> callback
) {
    (void)statement;
    (void)callback;
    throw std::logic_error("Not implement");
}

void
find_imports_in_statement(
    dtl::variant_ptr<const Statement> base_statement,
    std::function<void(const std::string&)> callback
) {
    if (auto statement = dtl::get_if<const AssignmentStatement*>(base_statement)) {
        find_imports_in_assignment_statement(*statement, callback);
        return;
    }

    if (auto statement = dtl::get_if<const UpdateStatement*>(base_statement)) {
        find_imports_in_update_statement(*statement, callback);
        return;
    }

    if (auto statement = dtl::get_if<const DeleteStatement*>(base_statement)) {
        find_imports_in_delete_statement(*statement, callback);
        return;
    }

    if (auto statement = dtl::get_if<const InsertStatement*>(base_statement)) {
        find_imports_in_insert_statement(*statement, callback);
        return;
    }

    if (auto statement = dtl::get_if<const ExportStatement*>(base_statement)) {
        find_imports_in_export_statement(*statement, callback);
        return;
    }

    if (auto statement = dtl::get_if<const BeginStatement*>(base_statement)) {
        find_imports_in_begin_statement(*statement, callback);
        return;
    }
}

void
find_imports_in_script(
    const Script& script,
    std::function<void(const std::string&)> callback
) {
    for (auto&& statement : script.statements) {
        find_imports_in_statement(borrow(statement), callback);
    }
}

// Note that this returns a vector rather than an unordered set.  Opening files
// in the same order as they are imported makes error reporting more predictable.
std::vector<std::string>
find_imports(dtl::ast::Script& script) {
    std::vector<std::string> imports;
    find_imports_in_script(script, [&](const std::string& value) {
        imports.push_back(value);
    });
    return imports;
}

} /* namespace ast */
} /* namespace dtl */
