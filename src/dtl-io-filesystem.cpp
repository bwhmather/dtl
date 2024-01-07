#include "dtl-io-filesystem.hpp"

#include <filesystem>
#include <memory>

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

#include "dtl-io.hpp"
#include "dtl-manifest.hpp"
#include "dtl-uuid.hpp"

namespace dtl {
namespace io {

/* === Importer ============================================================= */

class FilesystemImporter : public Importer {
    std::filesystem::path m_root;
    std::unordered_map<std::string, std::shared_ptr<arrow::Table>> m_cache;

  public:
    FilesystemImporter(const std::filesystem::path& root) :
        m_root(root) {}

    std::shared_ptr<arrow::Schema>
    import_schema(const std::string& name) override final {
        auto table = import_table(name);
        return table->schema();
    }

    std::shared_ptr<arrow::Table>
    import_table(const std::string& name) override final {
        std::shared_ptr<arrow::Table> table;
        auto cursor = m_cache.find(name);
        if (cursor == m_cache.end()) {
            auto input_path = m_root / (name + ".parquet");

            std::shared_ptr<arrow::io::ReadableFile> input_file;
            PARQUET_ASSIGN_OR_THROW(
                input_file, arrow::io::ReadableFile::Open(
                                input_path, arrow::default_memory_pool()
                            )
            );

            std::unique_ptr<parquet::arrow::FileReader> reader;
            PARQUET_THROW_NOT_OK(parquet::arrow::OpenFile(
                input_file, arrow::default_memory_pool(), &reader
            ));

            PARQUET_THROW_NOT_OK(reader->ReadTable(&table));

            m_cache[name] = table;
        } else {
            table = cursor->second;
        }

        return table;
    }
};

std::unique_ptr<Importer>
filesystem_importer(const std::filesystem::path& root) {
    return std::make_unique<FilesystemImporter>(root);
}

/* === Exporter ============================================================= */

class FilesystemExporter : public Exporter {
    std::filesystem::path m_root;

  public:
    FilesystemExporter(const std::filesystem::path& root) :
        m_root(root) {}

    void
    export_table(const std::string& name, std::shared_ptr<arrow::Table> table)
        override final {
        auto output_path = m_root / (name + ".parquet");

        std::shared_ptr<arrow::io::FileOutputStream> outfile;
        PARQUET_ASSIGN_OR_THROW(
            outfile, arrow::io::FileOutputStream::Open(output_path)
        );

        PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
            *table, arrow::default_memory_pool(), outfile, 65535
        ));
    }
};

std::unique_ptr<Exporter>
filesystem_exporter(const std::filesystem::path& root) {
    return std::make_unique<FilesystemExporter>(root);
}

/* === Tracer =============================================================== */

class FilesystemTracer : public Tracer {
    std::filesystem::path m_root;

  public:
    FilesystemTracer(const std::filesystem::path& root) :
        m_root(root) {}

    void
    write_manifest(const dtl::manifest::Manifest& manifest) override final {
        (void)manifest;
        throw "Not implemented error";
    }

    void
    write_array(dtl::UUID array_id, std::shared_ptr<const arrow::Array> array)
        override final {
        (void)array_id;
        (void)array;
        throw "Not implemented error";
    }
};

std::unique_ptr<Tracer>
filesystem_tracer(const std::filesystem::path& root) {
    return std::make_unique<FilesystemTracer>(root);
}

} /* namespace io */
} /* namespace dtl */
