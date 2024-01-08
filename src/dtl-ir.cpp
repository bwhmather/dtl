#include "dtl-ir.hpp"

#include <memory>

namespace dtl {
namespace ir {

/* === Tables =============================================================== */

std::shared_ptr<const Table>
Table::get_ptr() const {
    return shared_from_this();
}

void
TraceTable::accept(TableVisitor& visitor) const {
    visitor.visit_trace_table(*this);
}

std::shared_ptr<const TraceTable>
TraceTable::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const TraceTable>(expr_ptr);
}

void
ExportTable::accept(TableVisitor& visitor) const {
    visitor.visit_export_table(*this);
}

std::shared_ptr<const ExportTable>
ExportTable::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const ExportTable>(expr_ptr);
}

} /* namespace ir */
} /* namespace dtl */
