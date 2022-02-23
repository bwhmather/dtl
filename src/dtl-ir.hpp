#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "dtl-location.hpp"

namespace dtl {
namespace ir {

enum class DType {
    BOOL,
    INT32,
    DOUBLE,
    TEXT,
    BYTES,
};

/* === Expressions ========================================================== */

class ExpressionVisitor;

class Expression : public std::enable_shared_from_this<Expression> {
  public:
    DType dtype;

    virtual ~Expression() {};
    virtual void accept(ExpressionVisitor& visitor) const = 0;
};

class ImportExpression : public Expression {
  public:
    std::string location;
    std::string name;

    void accept(ExpressionVisitor& visitor) const override final;
};

class WhereExpression : public Expression {
  public:
    std::shared_ptr<const Expression> source;
    std::shared_ptr<const Expression> mask;

    void accept(ExpressionVisitor& visitor) const override final;
};

class PickExpression : public Expression {
  public:
    std::shared_ptr<const Expression> source;
    std::shared_ptr<const Expression> indexes;

    void accept(ExpressionVisitor& visitor) const override final;
};

class IndexExpression : public Expression {
  public:
    std::shared_ptr<const Expression> source;

    void accept(ExpressionVisitor& visitor) const override final;
};

class JoinLeftExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class JoinRightExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class AddExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class SubtractExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class MultiplyExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class DivideExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class ExpressionVisitor {
  public:
    virtual ~ExpressionVisitor() {};

    virtual void visit_import_expression(const ImportExpression& expression) = 0;
    virtual void visit_where_expression(const WhereExpression& expression) = 0;
    virtual void visit_pick_expression(const PickExpression& expression) = 0;
    virtual void visit_index_expression(const IndexExpression& expression) = 0;
    virtual void visit_join_left_expression(const JoinLeftExpression& expression) = 0;
    virtual void visit_join_right_expression(const JoinRightExpression& expression) = 0;
    virtual void visit_add_expression(const Expression& expression) = 0;
    virtual void visit_subtract_expression(const SubtractExpression& expression) = 0;
    virtual void visit_multiply_expression(const MultiplyExpression& expression) = 0;
    virtual void visit_divide_expression(const DivideExpression& expression) = 0;

    void visit(const Expression& expression) {
        expression.accept(*this);
    }
};

/* === Tables =============================================================== */

class Column {
  public:
    std::string name;
    std::unordered_set<std::string> namespaces;

    std::shared_ptr<const Expression> expression;
};

class Table {
  public:
    std::vector<Column> columns;
};

enum class TraceLevel {
    ASSERTION,
    STATEMENT,
    TABLE_EXPRESSION,
    COLUMN_EXPRESSION,
};

struct Trace {
    TraceLevel level;
    dtl::Location start;
    dtl::Location end;
    std::shared_ptr<Table> table;
};

struct Export {
    std::string name;
    std::shared_ptr<Table> table;
};


class Program {
  public:
    std::vector<Trace> traces;
    std::vector<Export> exports;
};

}  /* namespace ir */
}  /* namespace dtl */
