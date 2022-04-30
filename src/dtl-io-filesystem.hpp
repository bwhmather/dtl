#pragma once

#include <filesystem>
#include <memory>

#include "dtl-io.hpp"

namespace dtl {
namespace io {

std::unique_ptr<Importer> filesystem_importer(std::filesystem::path& root);
std::unique_ptr<Exporter> filesystem_exporter(std::filesystem::path& root);
std::unique_ptr<Tracer> filesystem_tracer(std::filesystem::path& root);

} /* namespace io */
} /* namespace dtl */
