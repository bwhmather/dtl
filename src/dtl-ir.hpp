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
    virtual ~Expression(){};

    virtual void
    accept(ExpressionVisitor& visitor) const = 0;

    std::shared_ptr<const Expression>
    get_ptr() const;
};

/* --- Shape Expressions ---------------------------------------------------- */

class ShapeExpression : public Expression {
  public:
    void
    accept(ExpressionVisitor& visitor) const override final;

    virtual void
    accept(ShapeExpressionVisitor& visitor) const = 0;

    std::shared_ptr<const ShapeExpression>
    get_ptr() const;
};

class ImportShapeExpression : public ShapeExpression {
  public:
    std::string location;

    void
    accept(ShapeExpressionVisitor& visitor) const override final;

    std::shared_ptr<const ImportShapeExpression>
    get_ptr() const;
};

class WhereShapeExpression : public ShapeExpression {
  public:
    std::shared_ptr<const ArrayExpression> mask;

    void
    accept(ShapeExpressionVisitor& visitor) const override final;

    std::shared_ptr<const WhereShapeExpression>
    get_ptr() const;
};

class JoinShapeExpression : public ShapeExpression {
  public:
    std::shared_ptr<const ShapeExpression> shape_a;
    std::shared_ptr<const ShapeExpression> shape_b;

    void
    accept(ShapeExpressionVisitor& visitor) const override final;

    std::shared_ptr<const JoinShapeExpression>
    get_ptr() const;
};

/* --- Array Expressions ---------------------------------------------------- */

class ArrayExpression : public Expression {
  public:
    DType dtype;
    std::shared_ptr<const ShapeExpression> shape;

    void
    accept(ExpressionVisitor& visitor) const override final;

    virtual void
    accept(ArrayExpressionVisitor& visitor) const = 0;

    std::shared_ptr<const ArrayExpression>
    get_ptr() const;
};

class ImportExpression : public ArrayExpression {
  public:
    std::string location;
    std::string name;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const ImportExpression>
    get_ptr() const;
};

class WhereExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> source;
    std::shared_ptr<const ArrayExpression> mask;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const WhereExpression>
    get_ptr() const;
};

class PickExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> source;
    std::shared_ptr<const ArrayExpression> indexes;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const PickExpression>
    get_ptr() const;
};

class IndexExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> source;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const IndexExpression>
    get_ptr() const;
};

class JoinLeftExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ShapeExpression> left;
    std::shared_ptr<const ShapeExpression> right;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const JoinLeftExpression>
    get_ptr() const;
};

class JoinRightExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ShapeExpression> left;
    std::shared_ptr<const ShapeExpression> right;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const JoinRightExpression>
    get_ptr() const;
};

class AddExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> left;
    std::shared_ptr<const ArrayExpression> right;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const AddExpression>
    get_ptr() const;
};

class SubtractExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> left;
    std::shared_ptr<const ArrayExpression> right;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const SubtractExpression>
    get_ptr() const;
};

class MultiplyExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> left;
    std::shared_ptr<const ArrayExpression> right;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const MultiplyExpression>
    get_ptr() const;
};

class DivideExpression : public ArrayExpression {
  public:
    std::shared_ptr<const ArrayExpression> left;
    std::shared_ptr<const ArrayExpression> right;

    void
    accept(ArrayExpressionVisitor& visitor) const override final;

    std::shared_ptr<const DivideExpression>
    get_ptr() const;
};

/* --- Visitors ------------------------------------------------------------- */

class ShapeExpressionVisitor {
  public:
    virtual ~ShapeExpressionVisitor(){};

    virtual void
    visit_import_shape_expression(const ImportShapeExpression& expression) = 0;
    virtual void
    visit_join_shape_expression(const JoinShapeExpression& expression) = 0;
    virtual void
    visit_where_shape_expression(const WhereShapeExpression& expression) = 0;
};

class ArrayExpressionVisitor {
  public:
    virtual ~ArrayExpressionVisitor(){};

    virtual void
    visit_import_expression(const ImportExpression& expression) = 0;

    virtual void
    visit_where_expression(const WhereExpression& expression) = 0;

    virtual void
    visit_pick_expression(const PickExpression& expression) = 0;

    virtual void
    visit_index_expression(const IndexExpression& expression) = 0;

    virtual void
    visit_join_left_expression(const JoinLeftExpression& expression) = 0;

    virtual void
    visit_join_right_expression(const JoinRightExpression& expression) = 0;

    virtual void
    visit_add_expression(const AddExpression& expression) = 0;

    virtual void
    visit_subtract_expression(const SubtractExpression& expression) = 0;

    virtual void
    visit_multiply_expression(const MultiplyExpression& expression) = 0;

    virtual void
    visit_divide_expression(const DivideExpression& expression) = 0;
};

class ExpressionVisitor :
    public ShapeExpressionVisitor,
    public ArrayExpressionVisitor {
  public:
    virtual ~ExpressionVisitor(){};
};

/* === Tables =============================================================== */

class TableVisitor;

struct Column {
    std::string name;
    std::shared_ptr<const ArrayExpression> expression;
};

class Table : public std::enable_shared_from_this<Table> {
  public:
    virtual ~Table(){};

    std::vector<Column> columns;

    virtual void
    accept(TableVisitor& visitor) const = 0;

    std::shared_ptr<const Table>
    get_ptr() const;
};

enum class TraceLevel {
    ASSERTION,
    STATEMENT,
    TABLE_EXPRESSION,
    COLUMN_EXPRESSION,
};

class TraceTable : public Table {
  public:
    TraceLevel level;
    dtl::Location start;
    dtl::Location end;

    void
    accept(TableVisitor& visitor) const override final;

    std::shared_ptr<const TraceTable>
    get_ptr() const;
};

class ExportTable : public Table {
  public:
    std::string name;

    void
    accept(TableVisitor& visitor) const override final;

    std::shared_ptr<const ExportTable>
    get_ptr() const;
};

class TableVisitor {
  public:
    virtual ~TableVisitor(){};

    virtual void
    visit_trace_table(const TraceTable& table) = 0;

    virtual void
    visit_export_table(const ExportTable& table) = 0;
};

} /* namespace ir */
} /* namespace dtl */
