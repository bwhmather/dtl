#pragma once

#include <memory>
#include <string>
#include <vector>

#include "dtl-ir.hpp"
#include "dtl-uuid.hpp"

namespace dtl {
namespace cmd {

class CommandVisitor;

struct Command {
  public:
    virtual ~Command() {};

    virtual void accept(CommandVisitor& visitor) const = 0;
};

class EvaluateArrayCommand : public Command {
  public:
    EvaluateArrayCommand(
        std::shared_ptr<const dtl::ir::ArrayExpression> expression
    );

    std::shared_ptr<const dtl::ir::ArrayExpression> expression;

    void accept(CommandVisitor& visitor) const override final;
};

class EvaluateShapeCommand : public Command {
  public:
    EvaluateShapeCommand(
        std::shared_ptr<const dtl::ir::ShapeExpression> expression
    );

    std::shared_ptr<const dtl::ir::ShapeExpression> expression;

    void accept(CommandVisitor& visitor) const override final;
};

class CollectArrayCommand : public Command {
  public:
    CollectArrayCommand(
        std::shared_ptr<const dtl::ir::ArrayExpression> expression
    );

    std::shared_ptr<const dtl::ir::ArrayExpression> expression;

    void accept(CommandVisitor& visitor) const override final;
};

class TraceArrayCommand : public Command {
  public:
    TraceArrayCommand(
        dtl::UUID uuid,
        std::shared_ptr<const dtl::ir::ArrayExpression> expression
    );

    dtl::UUID uuid;
    std::shared_ptr<const dtl::ir::ArrayExpression> expression;

    void accept(CommandVisitor& visitor) const override final;
};

class ExportTableCommand : public Command {
  public:
    ExportTableCommand(
        std::shared_ptr<const dtl::ir::ExportTable> table
    );

    std::shared_ptr<const dtl::ir::ExportTable> table;

    void accept(CommandVisitor& visitor) const override final;
};

class CommandVisitor {
  protected:
    virtual ~CommandVisitor() = default;

  public:
    virtual void visit_evaluate_array_command(
        const EvaluateArrayCommand& cmd
    ) = 0;

    virtual void visit_evaluate_shape_command(
        const EvaluateShapeCommand& cmd
    ) = 0;

    virtual void visit_collect_array_command(
        const CollectArrayCommand& cmd
    ) = 0;

    virtual void visit_trace_array_command(
        const TraceArrayCommand& cmd
    ) = 0;

    virtual void visit_export_table_command(
        const ExportTableCommand& cmd
    ) = 0;
};

}  /* namespace cmd */
}  /* namespace dtl */