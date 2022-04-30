#pragma once

#include <memory>
#include <string>

#include <arrow/api.h>

#include "dtl-location.hpp"

namespace dtl {
namespace io {

class Importer {
  public:
    virtual ~Importer() {};

    virtual std::shared_ptr<arrow::Schema> import_schema(std::string& name) = 0;
    virtual std::shared_ptr<arrow::Table> import_table(std::string& name) = 0;
};

class Exporter {
  public:
    virtual ~Exporter() {};

    virtual void export_table(std::string& name, std::shared_ptr<arrow::Table>) = 0;
};

class Tracer {
  public:
    virtual ~Tracer() {};

    virtual int write_array(std::shared_ptr<arrow::Array> array) = 0;
    virtual int write_snapshot(
        std::unordered_map<std::string, int> columns,
        dtl::Location start,
        dtl::Location end
    ) = 0;
    virtual int write_mapping(
        int mapping_array,
        int source,
        int target
    ) = 0;
};

} /* namespace io */
} /* namespace dtl */
