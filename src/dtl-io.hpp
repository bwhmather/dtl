#pragma once

#include <memory>
#include <string>

#include <arrow/api.h>

#include "dtl-location.hpp"
#include "dtl-manifest.hpp"
#include "dtl-uuid.hpp"

namespace dtl {
namespace io {

class Importer {
  public:
    virtual ~Importer(){};

    virtual std::shared_ptr<arrow::Schema>
    import_schema(const std::string& name) = 0;

    virtual std::shared_ptr<arrow::Table>
    import_table(const std::string& name) = 0;
};

class Exporter {
  public:
    virtual ~Exporter(){};

    virtual void
    export_table(
        const std::string& name, std::shared_ptr<arrow::Table> table
    ) = 0;
};

class Tracer {
  public:
    virtual ~Tracer(){};

    virtual void
    write_manifest(const dtl::manifest::Manifest& manifest) = 0;

    virtual void
    write_array(
        dtl::UUID array_id, std::shared_ptr<const arrow::Array> array
    ) = 0;
};

} /* namespace io */
} /* namespace dtl */
