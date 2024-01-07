#pragma once

#include <functional>

#include "dtl-ir.hpp"
#include "dtl-variant.tpp"

namespace dtl {
namespace ir {

void
for_each_direct_dependency(
    dtl::variant_ptr<const Expression> node, std::function<void(dtl::variant_ptr<const Expression>)> callback
);

} /* namespace ir */
} /* namespace dtl */
