#include "dtl-manifest.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct dtl_manifest *
dtl_manifest_create(void) {
    return calloc(1, sizeof(struct dtl_manifest));
}

void
dtl_manifest_destroy(struct dtl_manifest *manifest) {
    size_t i;

    assert(manifest != NULL);

    for (i = 0; i < manifest->num_sources; i++) {
        free(manifest->sources[i].text);
        free(manifest->sources[i].filename);
    }
    free(manifest->sources);

    for (i = 0; i < manifest->num_traces; i++) {
        free(manifest->traces[i].filename);
        free(manifest->traces[i].column);
    }
    free(manifest->traces);

    free(manifest->mappings);

    free(manifest);
}

void
dtl_manifest_add_source(struct dtl_manifest *manifest, char const *text, char const *filename) {
    struct dtl_manifest_source *source;

    assert(manifest != NULL);
    assert(text != NULL);
    assert(filename != NULL);

    manifest->sources = realloc(manifest->sources, sizeof(struct dtl_manifest_source) * manifest->num_sources + 1);
    manifest->num_sources += 1;

    source = &manifest->sources[manifest->num_sources - 1];
    source->text = strdup(text);
    source->filename = strdup(filename);
}

void
dtl_manifest_add_trace(
    struct dtl_manifest *manifest,
    char *filename,
    size_t start_offset,
    size_t end_offset,
    char *column,
    size_t array
) {
    struct dtl_manifest_trace *trace;

    assert(manifest != NULL);
    assert(filename != NULL);
    assert(start_offset < end_offset);
    assert(column != NULL);

    manifest->traces = realloc(manifest->traces, sizeof(struct dtl_manifest_trace) * manifest->num_traces + 1);
    manifest->num_traces += 1;

    trace = &manifest->traces[manifest->num_traces - 1];
    trace->filename = strdup(filename);
    trace->start_offset = start_offset;
    trace->end_offset = end_offset;
    trace->column = strdup(column);
    trace->array = array;
}

void
dtl_manifest_add_mapping(
    struct dtl_manifest *manifest,
    size_t src_array,
    size_t tgt_array,
    ssize_t src_index_array,
    ssize_t tgt_index_array
) {
    struct dtl_manifest_mapping *mapping;

    assert(manifest != NULL);
    assert(tgt_array > src_array);
    assert(src_index_array >= -1);
    assert(tgt_index_array >= -1);

    manifest->mappings = realloc(manifest->mappings, sizeof(struct dtl_manifest_mapping) * manifest->num_mappings + 1);
    manifest->num_mappings += 1;

    mapping = &manifest->mappings[manifest->num_mappings - 1];
    mapping->src_array = src_array;
    mapping->tgt_array = tgt_array;
    mapping->src_index_array = src_index_array;
    mapping->tgt_index_array = tgt_index_array;
}
