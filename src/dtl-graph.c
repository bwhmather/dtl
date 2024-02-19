#include "dtl-graph.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct dtl_node_header {
    uint32_t tag;
    uint32_t deps_end;
    char data[];
};

struct dtl_graph {
    char *nodes;
    size_t node_size;
    size_t nodes_length;
    size_t nodes_capacity;

    uint64_t *marks;
    uint32_t *rellocations;

    // Because I am a terrible person, nodes are referenced externally by a base pointer plus the
    // real node index.  This allows us to freely realloc without having to worry about invalidating
    // external pointers, and to check the generation without resorting to using struct values as
    // references.  Without this one bit of cuteness, this would be a very boring codebase.
    char *curr_base;
    char *prev_base;

    uint32_t *deps;
    size_t deps_length;
    size_t deps_capacity;

    bool writing;
};


static uint32_t
dtl_graph_index_for_node_ptr(struct dtl_graph *graph, struct dtl_node *node) {
    ptrdiff_t diff = (char *) node - graph->curr_base;
    assert(diff >= 0);
    assert((size_t) diff < graph->nodes_length);
    return (uint32_t) diff;
}

static dtl_node *
dtl_graph_node_ptr_for_index(struct dtl_graph *graph, uint32_t index) {
    assert(index >= 0);
    assert(index < graph->nodes_length);
    return (struct dtl_node *) (graph->curr_base + index);
}

static struct dtl_node_header *
dtl_graph_header_for_index(struct dtl_graph *graph, uint32_t index) {
    return nodes[(nodes->nodes_length + 1) * nodes->node_size];
}

static uint32_t *
dtl_graph_deps_for_index(struct dtl_graph *graph, uint32_t index) {
    uint32_t deps_start = 0;
    if (index > 0) {
        deps_start = dtl_graph_header_for_index(graph, index)->deps_end;
    }
    return &graph->deps[deps_start];
}

struct dtl_graph *
dtl_graph_new(size_t nodes_capacity, size_t deps_capacity, size_t node_data_size) {
    struct dtl_graph *graph = calloc(1, sizeof(struct dtl_graph));
    if (graph == NULL) {
        goto error;
    }

    // Align to 8 byte boundary.
    size_t node_size = (((sizeof(struct dtl_node_header) + node_data_size) + 7) / 8) * 8;

    graph->nodes = calloc(node_size, nodes_capacity);
    if (graph->nodes == NULL) {
        goto error;
    }
    graph->node_size = node_size;
    graph->nodes_length = 0;
    graph->nodes_capacity = nodes_capacity;

    graph->marks = calloc(sizeof(uint64_t), (nodes_capacity + 63) / 64);
    if (graph->marks == NULL) {
        goto error;
    }

    graph->rellocations  = calloc(sizeof(uint32_t), nodes_capacity);
    if (graph->rellocations == NULL) {
        goto error;
    }

    graph->deps = calloc(sizeof(uint32_t), deps_capacity);
    if (graph->deps == NULL) {
        goto error;
    }

    return graph;

error:
    if (graph != NULL) {
        free(graph->nodes);
        free(graph->marks);
        free(graph->rellocations);
        free(graph->deps);
    }
    free(graph);
    return NULL;
}

size_t
dtl_graph_get_num_nodes(struct dtl_graph *graph) {
    assert(graph != NULL);
    return graph->nodes_length;
}

size_t
dtl_graph_get_max_num_nodes(struct dtl_graph *graph) {
    assert(graph != NULL);
    return graph->nodes_capacity;
}

size_t
dtl_graph_get_num_dependencies(struct dtl_graph *graph) {
    assert(graph != NULL);
    return graph->deps_length;
}

size_t
dtl_graph_get_max_num_dependencies(struct dtl_graph *graph) {
    assert(graph != NULL);
    return graph->deps_capacity;
}

/* --- Generic struct dtl_node Construction ---------------------------------------------------------------- */

void
dtl_graph_cursor_begin(struct dtl_graph *graph, uint32_t tag) {
    assert(!graph->writing);
    assert(graph->nodes_length < graph->nodes_capacity);  // TODO soft error.

    graph->writing = true;

    struct dtl_node_header *header = dtl_graph_header_for_index(graph, nodes->nodes_length);
    memset(header, 0, nodes->node_size);

    header->tag = tag;
    if (graph->nodes_length) {
        header->deps_end = dtl_graph_header_for_index(graph,(graph->nodes_length - 1)->deps_end;
    }
}

void *
dtl_graph_cursor_data(struct dtl_graph *graph) {
    assert(graph->writing);

    struct dtl_node_header *header = nodes[(nodes->nodes_length + 1) * nodes->node_size];

    return header->data;
}

void
dtl_graph_create_node_add_dependency(struct dtl_graph *graph, struct dtl_node *dependency) {
    assert(graph->writing);

    graph->deps[graph


}

struct dtl_node *
dtl_graph_create_node_end(struct dtl_graph *graph) {

}


/* --- Data Access ------------------------------------------------------------------------------ */

uint32_t
dtl_graph_node_get_tag(struct dtl_graph *graph, struct dtl_node *node);

void *
dtl_graph_node_get_data(struct dtl_graph *graph, struct dtl_node *node);

size_t
dtl_graph_node_get_num_dependencies(struct dtl_graph *graph, struct dtl_node *node);

struct dtl_node *
dtl_graph_node_get_dependency(struct dtl_graph *graph, struct dtl_node *node, int dep);


/* --- Garbage Collection ----------------------------------------------------------------------- */

/**
 * Marks a node and all of its direct and indirect dependencies to be retained by the next call to
 * `dtl_graph_gc_collect`.
 *
 */
void
dtl_graph_gc_mark_root(struct dtl_graph *graph, struct dtl_node *node) {
    (void) graph;
    (void) node;
    // TODO
}

/**
 * Compacts the node heap, removing any node that is reachable from a merked root.  After calling
 * this, external references to nodes must be updated using `dtl_graph_gc_remap`.
 * Clears the set of marked roots.
 */
void
dtl_graph_gc_collect(struct dtl_graph *graph) {
    (void) graph;
    // TODO
}

/**
 * Takes a node pointer from the previous generation and maps it to its new location post-collection
 * Returns NULL if the node has been collected.
 * Behaviour is undefined if node is not from previous generation.
 */
struct dtl_node *
dtl_graph_gc_remap(struct dtl_graph *graph, struct dtl_node *node) {
    (void) graph;
    // TODO
    return node;
}


/* --- Iteration -------------------------------------------------------------------------------- */

void
dtl_graph_for_each_node(struct dtl_graph *graph, void (*callback)(struct dtl_node *, void *), void *data);

void
dtl_graph_for_each_node_reversed(struct dtl_graph *graph, void (*callback)(struct dtl_node *, void *), void *data);

void
dtl_graph_for_each_dependency(struct dtl_graph *graph, void (*callback)(struct dtl_node *, void *), void *data);

void
dtl_graph_for_each_dependency_reversed(struct dtl_graph *graph, void (*callback)(struct dtl_node *, void *), void *data);

size_t
dtl_graph_node_get_index(struct dtl_graph *graph, struct dtl_node *node) {

}


