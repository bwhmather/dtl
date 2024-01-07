#include "dtl-cmd.hpp"

namespace dtl {
namespace cmd {

EvaluateArrayCommand::EvaluateArrayCommand(
    dtl::shared_variant_ptr<const dtl::ir::ArrayExpression> expression_) :
    expression(expression_){};

void
EvaluateArrayCommand::accept(CommandVisitor& visitor) const {
    visitor.visit_evaluate_array_command(*this);
}

EvaluateShapeCommand::EvaluateShapeCommand(
    dtl::shared_variant_ptr<const dtl::ir::ShapeExpression> expression_) :
    expression(expression_){};

void
EvaluateShapeCommand::accept(CommandVisitor& visitor) const {
    visitor.visit_evaluate_shape_command(*this);
}

CollectArrayCommand::CollectArrayCommand(
    dtl::shared_variant_ptr<const dtl::ir::ArrayExpression> expression_) :
    expression(expression_){};

void
CollectArrayCommand::accept(CommandVisitor& visitor) const {
    visitor.visit_collect_array_command(*this);
}

TraceArrayCommand::TraceArrayCommand(
    dtl::UUID uuid_,
    dtl::shared_variant_ptr<const dtl::ir::ArrayExpression> expression_) :
    uuid(uuid_),
    expression(expression_){};

void
TraceArrayCommand::accept(CommandVisitor& visitor) const {
    visitor.visit_trace_array_command(*this);
}

ExportTableCommand::ExportTableCommand(
    std::shared_ptr<const dtl::ir::ExportTable> table_) :
    table(table_){};

void
ExportTableCommand::accept(CommandVisitor& visitor) const {
    visitor.visit_export_table_command(*this);
}

} /* namespace cmd */
} /* namespace dtl */
