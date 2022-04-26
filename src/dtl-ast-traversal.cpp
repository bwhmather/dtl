#include "dtl-ast-traversal.hpp"

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
        expr.name->accept(m_visitor);
    }
    virtual void visit_literal_expression(LiteralExpression& expr) override {
        expr.value->accept(m_visitor);
    }
    virtual void visit_function_call_expression(FunctionCallExpression& expr) override {
        for (auto&& argument : expr.arguments) {
            argument->accept(m_visitor);
        }
    }
    virtual void visit_add_expression(AddExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    virtual void visit_subtract_expression(SubtractExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    virtual void visit_multiply_expression(MultiplyExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }
    virtual void visit_divide_expression(DivideExpression& expr) override {
        expr.left->accept(m_visitor);
        expr.right->accept(m_visitor);
    }

    virtual void visit_wildcard_column_binding(WildcardColumnBinding& binding) override {
        (void) binding;
    }
    virtual void visit_implicit_column_binding(ImplicitColumnBinding& binding) override {
        binding.expression->accept(m_visitor);
    }
    virtual void visit_aliased_column_binding(AliasedColumnBinding& binding) override {
        binding.expression->accept(m_visitor);
    }

    virtual void visit_implicit_table_binding(ImplicitTableBinding& binding) override {
        binding.expression->accept(m_visitor);
    }
    virtual void visit_aliased_table_binding(AliasedTableBinding& binding) override {
        binding.expression->accept(m_visitor);
    }

    virtual void visit_join_on_constraint(JoinOnConstraint& constraint) override {
        constraint.predicate->accept(m_visitor);
    }
    virtual void visit_join_using_constraint(JoinUsingConstraint& constraint) override {
        for (auto&& column : constraint.columns) {
            column->accept(m_visitor);
        }
    }

    virtual void visit_select_expression(SelectExpression& expr) override {
        expr.distinct->accept(m_visitor);
        for (auto&& column : expr.columns) {
            column->accept(m_visitor);
        }
        expr.source->accept(m_visitor);
        for (auto&& join : expr.joins) {
            join->accept(m_visitor);
        }
        expr.where->accept(m_visitor);
        expr.group_by->accept(m_visitor);
    }
    virtual void visit_import_expression(ImportExpression& expr) override {
        expr.location->accept(m_visitor);
    }
    virtual void visit_table_reference_expression(TableReferenceExpression& expr) override {
        (void) expr;
    }

    virtual void visit_assignment_statement(AssignmentStatement& statement) override {
        statement.target->accept(m_visitor);
        statement.expression->accept(m_visitor);
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
        statement.location->accept(m_visitor);
        statement.expression->accept(m_visitor);
    }

    virtual void visit_table_name(TableName& table_name) override {
        (void) table_name;
    }
    virtual void visit_distinct_clause(DistinctClause& clause) override {
        (void) clause;
    }
    virtual void visit_from_clause(FromClause& clause) override {
        clause.binding->accept(m_visitor);
    }
    virtual void visit_join_clause(JoinClause& clause) override {
        clause.binding->accept(m_visitor);
        clause.constraint->accept(m_visitor);
    }
    virtual void visit_where_clause(WhereClause& clause) override {
        clause.predicate->accept(m_visitor);
    }
    virtual void visit_group_by_clause(GroupByClause& clause) override {
        for (auto&& expression : clause.pattern) {
            expression->accept(m_visitor);
        }
    }
    virtual void visit_script(Script& script) override {
        for (auto&& statement : script.statements) {
            statement->accept(m_visitor);
        }
    }
};

void visit_children(Node& node, NodeVisitor& visitor) {
    ChildNodeVisitor root_visitor(visitor);
    node.accept(root_visitor);
}

} /* namespace ast */
} /* namespace dtl */

