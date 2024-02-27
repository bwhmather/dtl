#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "dtl-location.hpp"
#include "dtl-variant.tpp"

namespace dtl {
namespace ir {

enum class DType {
    BOOL,
    INT32,
    DOUBLE,
    TEXT,
    BYTES,
    INDEX,
};

/* === Expressions ========================================================== */

struct BaseShapeExpression;

struct ImportShapeExpression;
struct WhereShapeExpression;
struct JoinShapeExpression;

typedef std::variant<
    ImportShapeExpression,
    WhereShapeExpression,
    JoinShapeExpression>
    ShapeExpression;

struct BaseArrayExpression;

struct ImportExpression;
struct WhereExpression;
struct PickExpression;
struct IndexExpression;
struct JoinLeftExpression;
struct JoinRightExpression;
struct EqualToExpression;
struct NotEqualToExpression;
struct LessThanExpression;
struct LessThanOrEqualExpression;
struct GreaterThanExpression;
struct GreaterThanOrEqualExpression;
struct AddExpression;
struct SubtractExpression;
struct MultiplyExpression;
struct DivideExpression;

typedef std::variant<
    ImportExpression,
    WhereExpression,
    PickExpression,
    IndexExpression,
    JoinLeftExpression,
    JoinRightExpression,
    EqualToExpression,
    NotEqualToExpression,
    LessThanExpression,
    LessThanOrEqualExpression,
    GreaterThanExpression,
    GreaterThanOrEqualExpression,
    AddExpression,
    SubtractExpression,
    MultiplyExpression,
    DivideExpression>
    ArrayExpression;

typedef merge<ShapeExpression, ArrayExpression> Expression;

/* --- Shape Expressions ---------------------------------------------------- */

struct BaseShapeExpression {};

struct ImportShapeExpression :
    public BaseShapeExpression,
    public std::enable_shared_from_this<ImportShapeExpression> {
    std::string location;
};

struct WhereShapeExpression :
    public BaseShapeExpression,
    public std::enable_shared_from_this<WhereShapeExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> mask;
};

struct JoinShapeExpression :
    public BaseShapeExpression,
    public std::enable_shared_from_this<JoinShapeExpression> {
    dtl::shared_variant_ptr<const ShapeExpression> shape_a;
    dtl::shared_variant_ptr<const ShapeExpression> shape_b;
};

/* --- Array Expressions ---------------------------------------------------- */

struct BaseArrayExpression {
    DType dtype;
    dtl::shared_variant_ptr<const ShapeExpression> shape;
};

struct ImportExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<ImportExpression> {
    std::string location;
    std::string name;
};

struct WhereExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<WhereExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> source;
    dtl::shared_variant_ptr<const ArrayExpression> mask;
};

struct PickExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<PickExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> source;
    dtl::shared_variant_ptr<const ArrayExpression> indexes;
};

struct IndexExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<IndexExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> source;
};

struct JoinLeftExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<JoinLeftExpression> {
    dtl::shared_variant_ptr<const ShapeExpression> left;
    dtl::shared_variant_ptr<const ShapeExpression> right;
};

struct JoinRightExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<JoinRightExpression> {
    dtl::shared_variant_ptr<const ShapeExpression> left;
    dtl::shared_variant_ptr<const ShapeExpression> right;
};

struct EqualToExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<EqualToExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

struct NotEqualToExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<NotEqualToExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

struct LessThanExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<LessThanExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

struct LessThanOrEqualExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<LessThanOrEqualExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

struct GreaterThanExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<GreaterThanExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

struct GreaterThanOrEqualExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<GreaterThanOrEqualExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

struct AddExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<AddExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

struct SubtractExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<SubtractExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

struct MultiplyExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<MultiplyExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

struct DivideExpression :
    public BaseArrayExpression,
    public std::enable_shared_from_this<DivideExpression> {
    dtl::shared_variant_ptr<const ArrayExpression> left;
    dtl::shared_variant_ptr<const ArrayExpression> right;
};

/* --- Helpers -------------------------------------------------------------- */

DType
expression_dtype(dtl::variant_ptr<const ArrayExpression> expression);

dtl::shared_variant_ptr<const ShapeExpression>
expression_shape(dtl::variant_ptr<const ArrayExpression> expression);

/* === Tables =============================================================== */

class TableVisitor;

struct Column {
    std::string name;
    dtl::shared_variant_ptr<const ArrayExpression> expression;
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
