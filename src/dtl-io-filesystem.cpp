extern "C" {
#include "dtl-io-filesystem.h"
}

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <arrow/type.h>
#include <filesystem>
#include <memory>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>
#include <string>
#include <unordered_map>
#include <utility>

extern "C" {
#include "dtl-io.h"
#include "dtl-array.h"
#include "dtl-value.h"
#include "dtl-dtype.h"
}


void
dtl_io_filesystem_set_error_from_arrow_status(struct dtl_error **error, arrow::Status status) {
    // TODO
    (void) error;
    (void) status;
}

/* === Importer ================================================================================= */

struct dtl_io_filesystem_column {
    std::string name;
    enum dtl_dtype dtype;
    // The importer will silently skip columns it doesn't understand.  This is the original column
    // index.
    size_t arrow_index;
};

struct dtl_io_filesystem_table {
    struct dtl_io_table base;

    std::shared_ptr<arrow::Table> arrow_table;
    std::vector<struct dtl_io_filesystem_column> columns;
};

struct dtl_io_filesystem_importer {
    struct dtl_io_importer base;

    std::filesystem::path root;
};

static size_t
dtl_io_filesystem_table_get_num_rows(struct dtl_io_table* table) {
    struct dtl_io_filesystem_table *fs_table;

    assert(table != NULL);
    assert(table->get_num_rows == dtl_io_filesystem_table_get_num_rows);

    fs_table = (struct dtl_io_filesystem_table *) table;

    return fs_table->arrow_table->num_rows();
}

static size_t
dtl_io_filesystem_table_get_num_columns(struct dtl_io_table* table) {
    struct dtl_io_filesystem_table *fs_table;

    assert(table != NULL);
    assert(table->get_num_columns == dtl_io_filesystem_table_get_num_columns);

    fs_table = (struct dtl_io_filesystem_table *) table;

    return fs_table->columns.size();
}

static char const*
dtl_io_filesystem_table_get_column_name(struct dtl_io_table* table, size_t index) {
    struct dtl_io_filesystem_table *fs_table;

    assert(table != NULL);
    assert(table->get_column_name == dtl_io_filesystem_table_get_column_name);

    fs_table = (struct dtl_io_filesystem_table *) table;
    assert(index < fs_table->columns.size());

    return fs_table->columns[index].name.c_str();
}

static enum dtl_dtype
dtl_io_filesystem_table_get_column_dtype(struct dtl_io_table* table, size_t index) {
    struct dtl_io_filesystem_table *fs_table;

    assert(table != NULL);
    assert(table->get_column_dtype == dtl_io_filesystem_table_get_column_dtype);

    fs_table = (struct dtl_io_filesystem_table *) table;
    assert(index < fs_table->columns.size());

    return fs_table->columns[index].dtype;
}

struct  dtl_io_filesystem_table_get_column_data_visitor : arrow::ScalarVisitor {
    enum dtl_dtype dtype;
    union dtl_value value;

    arrow::Status Visit(const arrow::Int32Scalar &scalar) {
        dtype = DTL_DTYPE_INT64;
        value.as_int64 = scalar.value;
        return arrow::Status::OK();
    }

    arrow::Status Visit(const arrow::Int64Scalar &scalar) {
        dtype = DTL_DTYPE_INT64;
        value.as_int64 = scalar.value;
        return arrow::Status::OK();
    }
};


static enum dtl_status
dtl_io_filesystem_table_get_column_data(
    struct dtl_io_table* table,
     size_t col_index,
      void *dest,
      size_t offset,
      size_t size,
       struct dtl_error **error
) {
    enum dtl_dtype dtype;
    struct dtl_io_filesystem_table *fs_table;
    std::shared_ptr<arrow::ChunkedArray> arrow_column;
    size_t i;
    arrow::Result<std::shared_ptr<arrow::Scalar>> arrow_scalar_result;
    struct dtl_io_filesystem_table_get_column_data_visitor visitor;
    arrow::Status arrow_status;

    assert(table != NULL);
    assert(table->get_column_data == dtl_io_filesystem_table_get_column_data);

    dtype = dtl_io_table_get_column_dtype(table, col_index);
    dtype = dtl_dtype_get_scalar_type(dtype);

    fs_table = (struct dtl_io_filesystem_table*)table;
    arrow_column = fs_table->arrow_table->column(col_index);

    for (i = offset; i < offset + size; i++) {
        // TODO this is fucking insane.  There must be a better way to do this.
        arrow_scalar_result = arrow_column->GetScalar(i);
        if (!arrow_scalar_result.ok()) {
            dtl_io_filesystem_set_error_from_arrow_status(error, arrow_scalar_result.status());
            return DTL_STATUS_ERROR;
        }
        arrow_status = arrow_scalar_result.ValueOrDie()->Accept(&visitor);
        if (!arrow_status.ok()) {
            dtl_io_filesystem_set_error_from_arrow_status(error, arrow_status);
            return DTL_STATUS_ERROR;
        }

        if (visitor.dtype != dtype) {
            // TODO set error.
            return DTL_STATUS_ERROR;
        }

        switch (dtype) {
        case DTL_DTYPE_INT64:
            ((int64_t*) dest)[offset + i] = visitor.value.as_int64;
            break;
        default:
            assert(false);
        }
    }

    return DTL_STATUS_OK;
}

static void
dtl_io_filesystem_table_destroy(struct dtl_io_table* table) {
    struct dtl_io_filesystem_table* fs_table;

    assert(table != NULL);
    assert(table->destroy == dtl_io_filesystem_table_destroy);

    fs_table = (struct dtl_io_filesystem_table*)table;
    delete fs_table;
}

struct dtl_io_table*
dtl_io_filesystem_importer_import_table(
    struct dtl_io_importer* importer,
    char const* name,
    struct dtl_error **error
) {
    struct dtl_io_filesystem_importer* fs_importer;
    arrow::Status status;
    std::shared_ptr<arrow::io::ReadableFile> input_file;
    std::unique_ptr<parquet::arrow::FileReader> arrow_reader;
    std::shared_ptr<arrow::Table> arrow_table;
    struct dtl_io_filesystem_table* fs_table;

    (void) error;

    fs_importer = (struct dtl_io_filesystem_importer*)importer;

    auto input_path = fs_importer->root / (std::string(name) + ".parquet");

    auto input_file_result = arrow::io::ReadableFile::Open(input_path, arrow::default_memory_pool());
    assert(input_file_result.ok()); // TODO
    input_file = input_file_result.ValueUnsafe();

    status = parquet::arrow::OpenFile(input_file, arrow::default_memory_pool(), &arrow_reader);
    if (!status.ok()) {
        dtl_io_filesystem_set_error_from_arrow_status(error, status);
        return NULL;
    }

    status = arrow_reader->ReadTable(&arrow_table);
    assert(status.ok()); // TODO

    fs_table = new struct dtl_io_filesystem_table;

    fs_table->base.get_num_rows = dtl_io_filesystem_table_get_num_rows;
    fs_table->base.get_num_columns = dtl_io_filesystem_table_get_num_columns;
    fs_table->base.get_column_name = dtl_io_filesystem_table_get_column_name;
    fs_table->base.get_column_dtype = dtl_io_filesystem_table_get_column_dtype;
    fs_table->base.get_column_data = dtl_io_filesystem_table_get_column_data;
    fs_table->base.destroy = dtl_io_filesystem_table_destroy;

    fs_table->arrow_table = arrow_table;

    auto arrow_schema = arrow_table->schema();
    for (int i = 0; i < arrow_schema->num_fields(); i++) {
        auto arrow_field = arrow_schema->field(i);
        struct dtl_io_filesystem_column column;
        column.name = arrow_field->name();
        // TODO arrow_field->type()) {

        column.dtype = DTL_DTYPE_INT64_ARRAY;
        column.arrow_index = i;

        fs_table->columns.push_back(column);
    }

    return &fs_table->base;
}

struct dtl_io_importer*
dtl_io_filesystem_importer_create(char const* root) {
    struct dtl_io_filesystem_importer* fs_importer;

    fs_importer = new struct dtl_io_filesystem_importer();
    fs_importer->base.import_table = dtl_io_filesystem_importer_import_table,
    fs_importer->root = root;

    return &fs_importer->base;
}

void
dtl_io_filesystem_importer_destroy(struct dtl_io_importer* importer) {
    struct dtl_io_filesystem_importer* fs_importer;

    if (importer == NULL) {
        return;
    }

    assert(importer->import_table == dtl_io_filesystem_importer_import_table);

    fs_importer = (struct dtl_io_filesystem_importer*)importer;
    delete fs_importer;
}

/* === Exporter ================================================================================= */

struct dtl_io_filesystem_exporter {
    struct dtl_io_exporter base;

    std::filesystem::path root;
};

static enum dtl_status
dtl_io_filesystem_exporter_export_table(
    struct dtl_io_exporter* exporter,
    char const* table_name,
    struct dtl_io_table* table,
    struct dtl_error **error
) {
    struct dtl_io_filesystem_exporter* fs_exporter;
    size_t num_rows;
    size_t col;
    size_t row;
    enum dtl_dtype col_dtype;
    char const* col_name;
    void* col_data = NULL;
    arrow::MemoryPool* pool;
    arrow::Status arrow_status;
    std::shared_ptr<arrow::Array> arrow_array;
    std::shared_ptr<arrow::Schema> arrow_schema;
    std::shared_ptr<arrow::Table> arrow_table;
    std::filesystem::path output_path;
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    enum dtl_status status;

    (void) error;

    assert(exporter != NULL);
    assert(exporter->export_table == dtl_io_filesystem_exporter_export_table);
    assert(table_name != NULL);
    assert(table != NULL);

    fs_exporter = (struct dtl_io_filesystem_exporter*)exporter;

    pool = arrow::default_memory_pool();

    num_rows = dtl_io_table_get_num_rows(table);

    std::vector<std::shared_ptr<arrow::Field>> schema_columns;
    std::vector<std::shared_ptr<arrow::Array>> table_columns;

    for (col = 0; col < dtl_io_table_get_num_columns(table); col++) {
        col_dtype = dtl_io_table_get_column_dtype(table, col);
        col_name = dtl_io_table_get_column_name(table, col);

        switch (col_dtype) {
        case DTL_DTYPE_BOOL_ARRAY: {
            arrow::BooleanBuilder builder(pool);
            arrow_status = builder.Resize(num_rows);
            assert(arrow_status.ok()); // TODO

            col_data = realloc(col_data, ((num_rows + 1) / 8) + 1);
            status = dtl_io_table_get_column_data(table, col, col_data, 0, num_rows, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }

            for (row = 0; row < num_rows; row++) {
                arrow_status = builder.Append(dtl_array_get_bool(col_data, row));
                assert(arrow_status.ok());
            }

            arrow_status = builder.Finish(&arrow_array);
            assert(arrow_status.ok()); // TODO

            break;
        }
        case DTL_DTYPE_INT64_ARRAY: {
            arrow::Int64Builder builder(pool);

            arrow_status = builder.Resize(num_rows);
            assert(arrow_status.ok()); // TODO

            col_data = realloc(col_data, num_rows * sizeof(int64_t));
            status = dtl_io_table_get_column_data(table, col, col_data, 0, num_rows, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }

            for (row = 0; row < num_rows; row++) {
                arrow_status = builder.Append(dtl_array_get_int64(col_data, row));
                assert(arrow_status.ok());
            }

            arrow_status = builder.Finish(&arrow_array);
            assert(arrow_status.ok()); // TODO

            break;
        }
        case DTL_DTYPE_DOUBLE_ARRAY:
        case DTL_DTYPE_STRING_ARRAY:
        case DTL_DTYPE_INDEX_ARRAY:
            assert(false); // TODO

        default:
            assert(false);
        }

        schema_columns.push_back(arrow::field(col_name, arrow_array->type()));
        table_columns.push_back(arrow_array);
    }
    free(col_data);

    arrow_schema = std::make_shared<arrow::Schema>(schema_columns);
    arrow_table = arrow::Table::Make(arrow_schema, table_columns);

    output_path = fs_exporter->root / (std::string(table_name) + ".parquet");

    auto outfile_result = arrow::io::FileOutputStream::Open(output_path);
    assert(outfile_result.ok());
    outfile = outfile_result.ValueUnsafe();

    arrow_status = parquet::arrow::WriteTable(*arrow_table, arrow::default_memory_pool(), outfile, 65535);
    assert(arrow_status.ok());

    return DTL_STATUS_OK;
}

struct dtl_io_exporter*
dtl_io_filesystem_exporter_create(char const* root) {
    struct dtl_io_filesystem_exporter* fs_exporter;

    fs_exporter = new struct dtl_io_filesystem_exporter();

    fs_exporter->base.export_table = dtl_io_filesystem_exporter_export_table;
    fs_exporter->root = root;

    return &fs_exporter->base;
}

void
dtl_io_filesystem_exporter_destroy(struct dtl_io_exporter* exporter) {
    struct dtl_io_filesystem_exporter* fs_exporter;

    if (exporter == NULL) {
        return;
    }

    assert(exporter->export_table == dtl_io_filesystem_exporter_export_table);

    fs_exporter = (struct dtl_io_filesystem_exporter*)exporter;
    delete fs_exporter;
}

/* === Tracer =================================================================================== */

struct dtl_io_filesystem_tracer {
    struct dtl_io_tracer base;

    std::filesystem::path root;
};

struct dtl_io_tracer*
dtl_io_filesystem_tracer_create(char const* root) {
    struct dtl_io_filesystem_tracer* fs_tracer;
    (void)root; // TODO

    fs_tracer = new struct dtl_io_filesystem_tracer();
    return &fs_tracer->base; // TODO
}

void
dtl_io_filesystem_tracer_destroy(struct dtl_io_tracer* tracer) {
    struct dtl_io_filesystem_tracer* fs_tracer;

    if (tracer == NULL) {
        return;
    }

    //    assert(tracer->write_manifest == dtl_io_filesystem_tracer_write_manifest); TODO
    //    assert(tracer->write_array == dtl_io_filesystem_tracer_write_array); TODO

    fs_tracer = (struct dtl_io_filesystem_tracer*)tracer;
    delete fs_tracer;
}
