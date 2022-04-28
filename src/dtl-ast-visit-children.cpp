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

    virtual void visit_string(String& string) override {
        (void) string;
    }

    virtual void visit_unqualified_column_name(UnqualifiedColumnName& column_name) override {
        (void) column_name;
    }
    virtual void visit_qualified_column_name(QualifiedColumnName& column_name) override {
        (void) column_name;
    }

    virtual void visit_column_reference_expression(ColumnReferenceExpression& expr) override {
        m_visitor.visit(*expr.name);
    }
    virtual void visit_literal_expression(LiteralExpression& expr) override {
        m_visitor.visit(*expr.value);
    }
    virtual void visit_function_call_expression(FunctionCallExpression& expr) override {
        for (auto&& argument : expr.arguments) {
            m_visitor.visit(*argument);
        }
    }
    virtual void visit_add_expression(AddExpression& expr) override {
        m_visitor.visit(*expr.left);
        m_visitor.visit(*expr.right);
    }
    virtual void visit_subtract_expression(SubtractExpression& expr) override {
        m_visitor.visit(*expr.left);
        m_visitor.visit(*expr.right);
    }
    virtual void visit_multiply_expression(MultiplyExpression& expr) override {
        m_visitor.visit(*expr.left);
        m_visitor.visit(*expr.right);
    }
    virtual void visit_divide_expression(DivideExpression& expr) override {
        m_visitor.visit(*expr.left);
        m_visitor.visit(*expr.right);
    }

    virtual void visit_wildcard_column_binding(WildcardColumnBinding& binding) override {
        (void) binding;
    }
    virtual void visit_implicit_column_binding(ImplicitColumnBinding& binding) override {
        m_visitor.visit(*binding.expression);
    }
    virtual void visit_aliased_column_binding(AliasedColumnBinding& binding) override {
        m_visitor.visit(*binding.expression);
    }

    virtual void visit_implicit_table_binding(ImplicitTableBinding& binding) override {
        m_visitor.visit(*binding.expression);
    }
    virtual void visit_aliased_table_binding(AliasedTableBinding& binding) override {
        m_visitor.visit(*binding.expression);
    }

    virtual void visit_join_on_constraint(JoinOnConstraint& constraint) override {
        m_visitor.visit(*constraint.predicate);
    }
    virtual void visit_join_using_constraint(JoinUsingConstraint& constraint) override {
        for (auto&& column : constraint.columns) {
            m_visitor.visit(*column);
        }
    }

    virtual void visit_select_expression(SelectExpression& expr) override {
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
    virtual void visit_import_expression(ImportExpression& expr) override {
        m_visitor.visit(*expr.location);
    }
    virtual void visit_table_reference_expression(TableReferenceExpression& expr) override {
        (void) expr;
    }

    virtual void visit_assignment_statement(AssignmentStatement& statement) override {
        m_visitor.visit(*statement.target);
        m_visitor.visit(*statement.expression);
    }
    virtual void visit_update_statement(UpdateStatement& statement) override {
        (void) statement;
        throw "Not implemented";
    }
    virtual void visit_delete_statement(DeleteStatement& statement) override {
        (void) statement;
        throw "Not implemented";
    }
    virtual void visit_insert_statement(InsertStatement& statement) override {
        (void) statement;
        throw "Not implemented";
    }
    virtual void visit_export_statement(ExportStatement& statement) override {
        m_visitor.visit(*statement.location);
        m_visitor.visit(*statement.expression);
    }

    virtual void visit_table_name(TableName& table_name) override {
        (void) table_name;
    }
    virtual void visit_distinct_clause(DistinctClause& clause) override {
        (void) clause;
    }
    virtual void visit_from_clause(FromClause& clause) override {
        m_visitor.visit(*clause.binding);
    }
    virtual void visit_join_clause(JoinClause& clause) override {
        m_visitor.visit(*clause.binding);
        m_visitor.visit(*clause.constraint);
    }
    virtual void visit_where_clause(WhereClause& clause) override {
        m_visitor.visit(*clause.predicate);
    }
    virtual void visit_group_by_clause(GroupByClause& clause) override {
        for (auto&& expression : clause.pattern) {
            m_visitor.visit(*expression);
        }
    }
    virtual void visit_script(Script& script) override {
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

