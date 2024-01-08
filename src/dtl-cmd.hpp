#pragma once

#include <memory>

#include "dtl-ir.hpp"
#include "dtl-uuid.hpp"
#include "dtl-variant.tpp"

namespace dtl {
namespace cmd {

class CommandVisitor;

struct Command {
  public:
    virtual ~Command(){};

    virtual void
    accept(CommandVisitor& visitor) const = 0;
};

class EvaluateArrayCommand : public Command {
  public:
    EvaluateArrayCommand(
        dtl::shared_variant_ptr<const dtl::ir::ArrayExpression> expression
    );

    dtl::shared_variant_ptr<const dtl::ir::ArrayExpression> expression;

    void
    accept(CommandVisitor& visitor) const override final;
};

class EvaluateShapeCommand : public Command {
  public:
    EvaluateShapeCommand(
        dtl::shared_variant_ptr<const dtl::ir::ShapeExpression> expression
    );

    dtl::shared_variant_ptr<const dtl::ir::ShapeExpression> expression;

    void
    accept(CommandVisitor& visitor) const override final;
};

class CollectArrayCommand : public Command {
  public:
    CollectArrayCommand(
        dtl::shared_variant_ptr<const dtl::ir::ArrayExpression> expression
    );

    dtl::shared_variant_ptr<const dtl::ir::ArrayExpression> expression;

    void
    accept(CommandVisitor& visitor) const override final;
};

class TraceArrayCommand : public Command {
  public:
    TraceArrayCommand(
        dtl::UUID uuid,
        dtl::shared_variant_ptr<const dtl::ir::ArrayExpression> expression
    );

    dtl::UUID uuid;
    dtl::shared_variant_ptr<const dtl::ir::ArrayExpression> expression;

    void
    accept(CommandVisitor& visitor) const override final;
};

class ExportTableCommand : public Command {
  public:
    ExportTableCommand(std::shared_ptr<const dtl::ir::ExportTable> table);

    std::shared_ptr<const dtl::ir::ExportTable> table;

    void
    accept(CommandVisitor& visitor) const override final;
};

class CommandVisitor {
  protected:
    virtual ~CommandVisitor() = default;

  public:
    virtual void
    visit_evaluate_array_command(const EvaluateArrayCommand& cmd) = 0;

    virtual void
    visit_evaluate_shape_command(const EvaluateShapeCommand& cmd) = 0;

    virtual void
    visit_collect_array_command(const CollectArrayCommand& cmd) = 0;

    virtual void
    visit_trace_array_command(const TraceArrayCommand& cmd) = 0;

    virtual void
    visit_export_table_command(const ExportTableCommand& cmd) = 0;
};

} /* namespace cmd */
} /* namespace dtl */
