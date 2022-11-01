#include "dtl-ast-visit-children.hpp"

#include <functional>
#include <memory>
#include <stddef.h>
#include <string>
#include <vector>

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

class ChildNodeVisitor : public NodeVisitor {
    std::function<void(const Node& node)> m_callback;

  public:
    ChildNodeVisitor(
        std::function<void(const Node&)> callback
    ) : m_callback(callback) {}

    void visit_string(const String& string) override {
        (void) string;
    }

    void visit_unqualified_column_name(const UnqualifiedColumnName& column_name) override {
        (void) column_name;
    }
    void visit_qualified_column_name(const QualifiedColumnName& column_name) override {
        (void) column_name;
    }

    void visit_column_reference_expression(const ColumnReferenceExpression& expr) override {
        m_callback(*expr.name);
    }
    void visit_literal_expression(const LiteralExpression& expr) override {
        m_callback(*expr.value);
    }
    void visit_function_call_expression(const FunctionCallExpression& expr) override {
        for (auto&& argument : expr.arguments) {
            m_callback(*argument);
        }
    }
    void visit_equal_to_expression(const EqualToExpression& expr) override {
        m_callback(*expr.left);
        m_callback(*expr.right);
    }
    void visit_less_than_expression(const LessThanExpression& expr) override {
        m_callback(*expr.left);
        m_callback(*expr.right);
    }
    void visit_less_than_equal_expression(const LessThanEqualExpression& expr) override {
        m_callback(*expr.left);
        m_callback(*expr.right);
    }
    void visit_greater_than_expression(const GreaterThanExpression& expr) override {
        m_callback(*expr.left);
        m_callback(*expr.right);
    }
    void visit_greater_than_equal_expression(const GreaterThanEqualExpression& expr) override {
        m_callback(*expr.left);
        m_callback(*expr.right);
    }
    void visit_add_expression(const AddExpression& expr) override {
        m_callback(*expr.left);
        m_callback(*expr.right);
    }
    void visit_subtract_expression(const SubtractExpression& expr) override {
        m_callback(*expr.left);
        m_callback(*expr.right);
    }
    void visit_multiply_expression(const MultiplyExpression& expr) override {
        m_callback(*expr.left);
        m_callback(*expr.right);
    }
    void visit_divide_expression(const DivideExpression& expr) override {
        m_callback(*expr.left);
        m_callback(*expr.right);
    }

    void visit_wildcard_column_binding(const WildcardColumnBinding& binding) override {
        (void) binding;
    }
    void visit_implicit_column_binding(const ImplicitColumnBinding& binding) override {
        m_callback(*binding.expression);
    }
    void visit_aliased_column_binding(const AliasedColumnBinding& binding) override {
        m_callback(*binding.expression);
    }

    void visit_implicit_table_binding(const ImplicitTableBinding& binding) override {
        m_callback(*binding.expression);
    }
    void visit_aliased_table_binding(const AliasedTableBinding& binding) override {
        m_callback(*binding.expression);
    }

    void visit_join_on_constraint(const JoinOnConstraint& constraint) override {
        m_callback(*constraint.predicate);
    }
    void visit_join_using_constraint(const JoinUsingConstraint& constraint) override {
        for (auto&& column : constraint.columns) {
            m_callback(*column);
        }
    }

    void visit_select_expression(const SelectExpression& expr) override {
        if (expr.distinct) {
            m_callback(*expr.distinct);
        }
        for (auto&& column : expr.columns) {
            m_callback(*column);
        }
        m_callback(*expr.source);
        for (auto&& join : expr.joins) {
            m_callback(*join);
        }
        if (expr.where) {
            m_callback(*expr.where);
        }
        if (expr.group_by) {
            m_callback(*expr.group_by);
        }
    }
    void visit_import_expression(const ImportExpression& expr) override {
        m_callback(*expr.location);
    }
    void visit_table_reference_expression(const TableReferenceExpression& expr) override {
        (void) expr;
    }

    void visit_assignment_statement(const AssignmentStatement& statement) override {
        m_callback(*statement.target);
        m_callback(*statement.expression);
    }
    void visit_update_statement(const UpdateStatement& statement) override {
        (void) statement;
        throw "Not implemented";
    }
    void visit_delete_statement(const DeleteStatement& statement) override {
        (void) statement;
        throw "Not implemented";
    }
    void visit_insert_statement(const InsertStatement& statement) override {
        (void) statement;
        throw "Not implemented";
    }
    void visit_export_statement(const ExportStatement& statement) override {
        m_callback(*statement.location);
        m_callback(*statement.expression);
    }
    void visit_begin_statement(const BeginStatement& statement) override {
        (void) statement;
    }
    void visit_table_name(const TableName& table_name) override {
        (void) table_name;
    }
    void visit_distinct_clause(const DistinctClause& clause) override {
        (void) clause;
    }
    void visit_from_clause(const FromClause& clause) override {
        m_callback(*clause.binding);
    }
    void visit_join_clause(const JoinClause& clause) override {
        m_callback(*clause.binding);
        m_callback(*clause.constraint);
    }
    void visit_where_clause(const WhereClause& clause) override {
        m_callback(*clause.predicate);
    }
    void visit_group_by_clause(const GroupByClause& clause) override {
        for (auto&& expression : clause.pattern) {
            m_callback(*expression);
        }
    }
    void visit_script(const Script& script) override {
        for (auto&& statement : script.statements) {
            m_callback(*statement);
        }
    }
};

void
visit_children(
    const Node& node, std::function<void(const Node&)> callback
) {
    ChildNodeVisitor root_visitor(callback);
    node.accept(root_visitor);
}

void visit_children(const Node& node, NodeVisitor& visitor) {
    ChildNodeVisitor root_visitor([&](const Node& child_node) {
        child_node.accept(visitor);
    });
    node.accept(root_visitor);
}

} /* namespace ast */
} /* namespace dtl */

