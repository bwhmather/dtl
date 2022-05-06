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

    void visit_string(String& string) override {
        (void) string;
    }

    void visit_unqualified_column_name(UnqualifiedColumnName& column_name) override {
        (void) column_name;
    }
    void visit_qualified_column_name(QualifiedColumnName& column_name) override {
        (void) column_name;
    }

    void visit_column_reference_expression(ColumnReferenceExpression& expr) override {
        m_visitor.visit(*expr.name);
    }
    void visit_literal_expression(LiteralExpression& expr) override {
        m_visitor.visit(*expr.value);
    }
    void visit_function_call_expression(FunctionCallExpression& expr) override {
        for (auto&& argument : expr.arguments) {
            m_visitor.visit(*argument);
        }
    }
    void visit_add_expression(AddExpression& expr) override {
        m_visitor.visit(*expr.left);
        m_visitor.visit(*expr.right);
    }
    void visit_subtract_expression(SubtractExpression& expr) override {
        m_visitor.visit(*expr.left);
        m_visitor.visit(*expr.right);
    }
    void visit_multiply_expression(MultiplyExpression& expr) override {
        m_visitor.visit(*expr.left);
        m_visitor.visit(*expr.right);
    }
    void visit_divide_expression(DivideExpression& expr) override {
        m_visitor.visit(*expr.left);
        m_visitor.visit(*expr.right);
    }

    void visit_wildcard_column_binding(WildcardColumnBinding& binding) override {
        (void) binding;
    }
    void visit_implicit_column_binding(ImplicitColumnBinding& binding) override {
        m_visitor.visit(*binding.expression);
    }
    void visit_aliased_column_binding(AliasedColumnBinding& binding) override {
        m_visitor.visit(*binding.expression);
    }

    void visit_implicit_table_binding(ImplicitTableBinding& binding) override {
        m_visitor.visit(*binding.expression);
    }
    void visit_aliased_table_binding(AliasedTableBinding& binding) override {
        m_visitor.visit(*binding.expression);
    }

    void visit_join_on_constraint(JoinOnConstraint& constraint) override {
        m_visitor.visit(*constraint.predicate);
    }
    void visit_join_using_constraint(JoinUsingConstraint& constraint) override {
        for (auto&& column : constraint.columns) {
            m_visitor.visit(*column);
        }
    }

    void visit_select_expression(SelectExpression& expr) override {
        if (expr.distinct) {
            m_visitor.visit(*expr.distinct);
        }
        for (auto&& column : expr.columns) {
            m_visitor.visit(*column);
        }
        m_visitor.visit(*expr.source);
        for (auto&& join : expr.joins) {
            m_visitor.visit(*join);
        }
        if (expr.where) {
            m_visitor.visit(*expr.where);
        }
        if (expr.group_by) {
            m_visitor.visit(*expr.group_by);
        }
    }
    void visit_import_expression(ImportExpression& expr) override {
        m_visitor.visit(*expr.location);
    }
    void visit_table_reference_expression(TableReferenceExpression& expr) override {
        (void) expr;
    }

    void visit_assignment_statement(AssignmentStatement& statement) override {
        m_visitor.visit(*statement.target);
        m_visitor.visit(*statement.expression);
    }
    void visit_update_statement(UpdateStatement& statement) override {
        (void) statement;
        throw "Not implemented";
    }
    void visit_delete_statement(DeleteStatement& statement) override {
        (void) statement;
        throw "Not implemented";
    }
    void visit_insert_statement(InsertStatement& statement) override {
        (void) statement;
        throw "Not implemented";
    }
    void visit_export_statement(ExportStatement& statement) override {
        m_visitor.visit(*statement.location);
        m_visitor.visit(*statement.expression);
    }

    void visit_table_name(TableName& table_name) override {
        (void) table_name;
    }
    void visit_distinct_clause(DistinctClause& clause) override {
        (void) clause;
    }
    void visit_from_clause(FromClause& clause) override {
        m_visitor.visit(*clause.binding);
    }
    void visit_join_clause(JoinClause& clause) override {
        m_visitor.visit(*clause.binding);
        m_visitor.visit(*clause.constraint);
    }
    void visit_where_clause(WhereClause& clause) override {
        m_visitor.visit(*clause.predicate);
    }
    void visit_group_by_clause(GroupByClause& clause) override {
        for (auto&& expression : clause.pattern) {
            m_visitor.visit(*expression);
        }
    }
    void visit_script(Script& script) override {
        for (auto&& statement : script.statements) {
            m_visitor.visit(*statement);
        }
    }
};

void visit_children(Node& node, NodeVisitor& visitor) {
    ChildNodeVisitor root_visitor(visitor);
    root_visitor.visit(node);
}

} /* namespace ast */
} /* namespace dtl */

