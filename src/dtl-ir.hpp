#pragma once

namespace dtl {
namespace ir {

enum DType {
    BOOL,
    INT32,
    DOUBLE,
    TEXT,
    BYTES,
}

/* === Expressions ========================================================== */

class ExpressionVisitor;

class Expression : public std::enable_shared_from_this<Node> {
  public:
    DType dtype;

    virtual void accept(ExpressionVisitor& visitor) = 0;
}

class ImportExpression : public Expression {
  public:
    std::string location;
    std::string name;

    void accept(ExpressionVisitor& visitor) override final;
}

class WhereExpression : public Expression {
  public:
    std::shared_ptr<const Expression> source;
    std::shared_ptr<const Expression> mask;

    void accept(ExpressionVisitor& visitor) override final;
}

class PickExpression : public Expression {
  public:
    std::shared_ptr<const Expression> source;
    std::shared_ptr<const Expression> indexes;

    void accept(ExpressionVisitor& visitor) override final;
}

class IndexExpression : public Expression {
  public:
    std::shared_ptr<const Expression> source;

    void accept(ExpressionVisitor& visitor) override final;
}

class JoinLeftExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) override final;
}

class JoinRightExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) override final;
}

class AddExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) override final;
}

class SubtractExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) override final;
}

class MultiplyExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) override final;
}

class DivideExpression : public Expression {
  public:
    std::shared_ptr<const Expression> left;
    std::shared_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) override final;
}

class ExpressionVisitor {
  public:
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
}

class Table {
  public:
    std::vector<const Column> columns;
}

class Program {
    std::vector<const Table> tables;
    std::unordered_map<std::string, const Table> exports;
}

}  /* namespace ir */
}  /* namespace dtl */
