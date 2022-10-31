#pragma once

#include <string>

#include "dtl-io.hpp"

namespace dtl {
namespace eval {

void run(
    std::string source,
    dtl::io::Importer& importer,
    dtl::io::Exporter& exporter,
    dtl::io::Tracer& tracer
);

}  /* namespace eval */
}  /* namespace dtl */

