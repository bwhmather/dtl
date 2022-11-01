#include "dtl-ast-visit-children.hpp"

#include <stddef.h>
#include <vector>
#include <string>
#include <memory>

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

class ChildNodeVisitor : public NodeVisitor {
    NodeVisitor& m_visitor;

  public:
    ChildNodeVisitor(NodeVisitor& visitor) : m_visitor(visitor) {}

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
        expr.name->accept(m_visitor);
    }
    void visit_literal_expression(const LiteralExpression& expr) override {
        expr.value->accept(m_visitor);
    }
    void visit_function_call_expression(const FunctionCallExpression& expr) override {
        for (auto&& argument : expr.arguments) {
            argument->accept(m_visitor);
        }
    }
    void visit_equal_to_expression(const EqualToExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    void visit_less_than_expression(const LessThanExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    void visit_less_than_equal_expression(const LessThanEqualExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    void visit_greater_than_expression(const GreaterThanExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    void visit_greater_than_equal_expression(const GreaterThanEqualExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    void visit_add_expression(const AddExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    void visit_subtract_expression(const SubtractExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    void visit_multiply_expression(const MultiplyExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    void visit_divide_expression(const DivideExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }

    void visit_wildcard_column_binding(const WildcardColumnBinding& binding) override {
        (void) binding;
    }
    void visit_implicit_column_binding(const ImplicitColumnBinding& binding) override {
        binding.expression->accept(m_visitor);
    }
    void visit_aliased_column_binding(const AliasedColumnBinding& binding) override {
        binding.expression->accept(m_visitor);
    }

    void visit_implicit_table_binding(const ImplicitTableBinding& binding) override {
        binding.expression->accept(m_visitor);
    }
    void visit_aliased_table_binding(const AliasedTableBinding& binding) override {
        binding.expression->accept(m_visitor);
    }

    void visit_join_on_constraint(const JoinOnConstraint& constraint) override {
        constraint.predicate->accept(m_visitor);
    }
    void visit_join_using_constraint(const JoinUsingConstraint& constraint) override {
        for (auto&& column : constraint.columns) {
            column->accept(m_visitor);
        }
    }

    void visit_select_expression(const SelectExpression& expr) override {
        if (expr.distinct) {
            expr.distinct->accept(m_visitor);
        }
        for (auto&& column : expr.columns) {
            column->accept(m_visitor);
        }
        expr.source->accept(m_visitor);
        for (auto&& join : expr.joins) {
            join->accept(m_visitor);
        }
        if (expr.where) {
            expr.where->accept(m_visitor);
        }
        if (expr.group_by) {
            expr.group_by->accept(m_visitor);
        }
    }
    void visit_import_expression(const ImportExpression& expr) override {
        expr.location->accept(m_visitor);
    }
    void visit_table_reference_expression(const TableReferenceExpression& expr) override {
        (void) expr;
    }

    void visit_assignment_statement(const AssignmentStatement& statement) override {
        statement.target->accept(m_visitor);
        statement.expression->accept(m_visitor);
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
        statement.location->accept(m_visitor);
        statement.expression->accept(m_visitor);
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
        clause.binding->accept(m_visitor);
    }
    void visit_join_clause(const JoinClause& clause) override {
        clause.binding->accept(m_visitor);
        clause.constraint->accept(m_visitor);
    }
    void visit_where_clause(const WhereClause& clause) override {
        clause.predicate->accept(m_visitor);
    }
    void visit_group_by_clause(const GroupByClause& clause) override {
        for (auto&& expression : clause.pattern) {
            expression->accept(m_visitor);
        }
    }
    void visit_script(const Script& script) override {
        for (auto&& statement : script.statements) {
            statement->accept(m_visitor);
        }
    }
};

void visit_children(const Node& node, NodeVisitor& visitor) {
    ChildNodeVisitor root_visitor(visitor);
    node.accept(root_visitor);
}

} /* namespace ast */
} /* namespace dtl */

