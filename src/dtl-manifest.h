#pragma once

#include <stddef.h>
#include <sys/types.h>

struct dtl_manifest_source {
    char *text;
    char *filename;
};

struct dtl_manifest_trace {
    char *filename;
    size_t start_offset;
    size_t end_offset;
    char *column;
    size_t array;
};

struct dtl_manifest_mapping {
    size_t src_array;
    size_t tgt_array;

    ssize_t src_index_array;
    ssize_t tgt_index_array;
};

struct dtl_manifest {
    struct dtl_manifest_source *sources;
    size_t num_sources;

    struct dtl_manifest_trace *traces;
    size_t num_traces;

    struct dtl_manifest_mapping *mappings;
    size_t num_mappings;
};

struct dtl_manifest *
dtl_manifest_create(void);

void
dtl_manifest_destroy(struct dtl_manifest *manifest);

void
dtl_manifest_add_source(struct dtl_manifest *manifest, char const *text, char const *filename);

void
dtl_manifest_add_trace(
    struct dtl_manifest *manifest,
    char *filename,
    size_t start_offset,
    size_t end_offset,
    char *column,
    size_t array
);

void
dtl_manifest_add_mapping(
    struct dtl_manifest *manifest,
    size_t src_array,
    size_t tgt_array,
    ssize_t src_index_array,
    ssize_t tgt_index_array
);
