#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "dtl-error.h"
#include "dtl-eval.h"
#include "dtl-io-filesystem.h"
#include "dtl-io.h"

char *
dtl_read_file(int f) {
    int errsv;
    char *buffer = NULL;
    size_t capacity = 1024;
    size_t size = 0;
    ssize_t n;

    errsv = errno;
    errno = 0;

    buffer = malloc(capacity);
    assert(buffer != NULL);

    while (true) {
        assert(size < capacity);

        if (size == capacity - 1) {
            capacity *= 3;
            capacity /= 2;
            buffer = realloc(buffer, capacity);
            assert(buffer != NULL);
        }

        n = read(f, buffer + size, capacity - size - 1);
        if (n == 0) {
            break;
        }
        if (n == -1 && errno != EAGAIN) {
            free(buffer);
            return NULL;
        }

        size += n;
    }

    buffer[size] = '\0';

    errno = errsv;
    return buffer;
}

int
main(int argc, char **argv) {
    char const *source_path;
    char const *input_path;
    char const *output_path;
    char const *trace_path;
    int source_file;
    char *source;
    struct dtl_io_importer *importer;
    struct dtl_io_exporter *exporter;
    struct dtl_io_tracer *tracer;
    struct dtl_error *error = NULL;
    enum dtl_status status;

    if (argc != 5) {
        return 1;
    }

    source_path = argv[1];
    input_path = argv[2];
    output_path = argv[3];
    trace_path = argv[4];

    source_file = open(source_path, O_CLOEXEC);
    if (source_file == -1) {
        return 1;
    }

    source = dtl_read_file(source_file);
    assert(source != NULL);

    importer = dtl_io_filesystem_importer_create(input_path);
    exporter = dtl_io_filesystem_exporter_create(output_path);
    tracer = dtl_io_filesystem_tracer_create(trace_path);

    status = dtl_eval(source, importer, exporter, tracer, &error);
    if (status != DTL_STATUS_OK) {
        fprintf(stderr, "error: %s\n", error->message);
        dtl_clear_error(&error);
        return 1;
    }

    return 0;
}
