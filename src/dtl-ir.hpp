#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "dtl-location.hpp"

namespace dtl {
namespace ir {

class ExpressionVisitor;
class ShapeExpressionVisitor;
class ArrayExpressionVisitor;

class Expression;
class ShapeExpression;
class ArrayExpression;

enum class DType {
    BOOL,
    INT32,
    DOUBLE,
    TEXT,
    BYTES,
    INDEX,
};

/* === Expressions ========================================================== */

class Expression : public std::enable_shared_from_this<Expression> {
  public:
    virtual ~Expression() {};

  private:
    friend class ExpressionVisitor;
    virtual void accept(ExpressionVisitor& visitor) const = 0;
};

/* --- Shape Expressions ---------------------------------------------------- */

class ShapeExpression : public Expression {
  private:
    void accept(ExpressionVisitor& visitor) const override final;

    friend class ShapeExpressionVisitor;
    virtual void accept(ShapeExpressionVisitor& visitor) const = 0;
};

class ImportShapeExpression : public ShapeExpression {
  public:
    std::string location;

  private:
    void accept(ShapeExpressionVisitor& visitor) const override final;
};

class WhereShapeExpression : public ShapeExpression {
  public:
    std::shared_ptr<const ArrayExpression> mask;

  private:
    void accept(ShapeExpressionVisitor& visitor) const override final;
};

class JoinShapeExpression : public ShapeExpression {
  public:
    std::shared_ptr<const ShapeExpression> shape_a;
    std::shared_ptr<const ShapeExpression> shape_b;

  private:
    void accept(ShapeExpressionVisitor& visitor) const override final;
};

/* --- Array Expressions ---------------------------------------------------- */

class ArrayExpression : public Expression {
  public:
    DType dtype;
    std::shared_ptr<const ShapeExpression> shape;

  private:
    void accept(ExpressionVisitor& visitor) const override final;

    friend class ArrayExpressionVisitor;
    virtual void accept(ArrayExpressionVisitor& visitor) const = 0;
};

class ImportExpression : public ArrayExpression {
  public:
    std::string location;
    std::string name;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

class WhereExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> source;
    std::shared_ptr<const ArrayExpression> mask;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

class PickExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> source;
    std::shared_ptr<const ArrayExpression> indexes;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

class IndexExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> source;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

class JoinLeftExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ShapeExpression> left;
    std::shared_ptr<const ShapeExpression> right;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

class JoinRightExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ShapeExpression> left;
    std::shared_ptr<const ShapeExpression> right;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

class AddExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> left;
    std::shared_ptr<const ArrayExpression> right;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

class SubtractExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> left;
    std::shared_ptr<const ArrayExpression> right;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

class MultiplyExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> left;
    std::shared_ptr<const ArrayExpression> right;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

class DivideExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> left;
    std::shared_ptr<const ArrayExpression> right;

  private:
    void accept(ArrayExpressionVisitor& visitor) const override final;
};

/* --- Visitors ------------------------------------------------------------- */

class ShapeExpressionVisitor {
  public:
    virtual ~ShapeExpressionVisitor() {};

    virtual void visit_import_shape_expression(
        const ImportShapeExpression& expression
    ) = 0;
    virtual void visit_join_shape_expression(
        const JoinShapeExpression& expression
    ) = 0;
    virtual void visit_where_shape_expression(
        const WhereShapeExpression& expression
    ) = 0;

    void visit(const ShapeExpression& expression);
};

class ArrayExpressionVisitor {
  public:
    virtual ~ArrayExpressionVisitor() {};

    virtual void visit_import_expression(const ImportExpression& expression) = 0;
    virtual void visit_where_expression(const WhereExpression& expression) = 0;
    virtual void visit_pick_expression(const PickExpression& expression) = 0;
    virtual void visit_index_expression(const IndexExpression& expression) = 0;
    virtual void visit_join_left_expression(const JoinLeftExpression& expression) = 0;
    virtual void visit_join_right_expression(const JoinRightExpression& expression) = 0;
    virtual void visit_add_expression(const AddExpression& expression) = 0;
    virtual void visit_subtract_expression(const SubtractExpression& expression) = 0;
    virtual void visit_multiply_expression(const MultiplyExpression& expression) = 0;
    virtual void visit_divide_expression(const DivideExpression& expression) = 0;

    void visit(const ArrayExpression& expression);
};

class ExpressionVisitor : public ShapeExpressionVisitor, public ArrayExpressionVisitor {
  public:
    using ShapeExpressionVisitor::visit;
    using ArrayExpressionVisitor::visit;

    void visit(const Expression& expression);
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
