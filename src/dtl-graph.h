#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


struct dtl_graph;
struct dtl_node;


struct dtl_graph *
dtl_graph_create(size_t node_capacity, size_t link_capacity);

void
dtl_graph_destroy(struct dtl_graph *graph);

size_t
dtl_graph_get_num_nodes(struct dtl_graph *graph);

size_t
dtl_graph_get_max_num_nodes(struct dtl_graph *graph);

size_t
dtl_graph_get_num_dependenciess(struct dtl_graph *graph);

size_t
dtl_graph_get_max_num_dependenciess(struct dtl_graph *graph);


/* --- Generic struct dtl_node Construction ---------------------------------------------------------------- */

void
dtl_graph_cursor_begin(struct dtl_graph *graph, uint32_t type);

void *
dtl_graph_cursor_data(struct dtl_graph *graph);

void
dtl_graph_cursor_add_dependency(struct dtl_graph *graph, struct dtl_node *dependency);

struct dtl_node *
dtl_graph_cursor_commit(struct dtl_graph *graph);


/* --- Data Access ------------------------------------------------------------------------------ */

uint32_t
dtl_graph_node_get_tag(struct dtl_graph *graph, struct dtl_node *node);

void *
dtl_graph_node_get_data(struct dtl_graph *graph, struct dtl_node *node);

/**
 * Returns the number of _direct_ dependencies of the given node.
 */
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
dtl_graph_gc_mark_root(struct dtl_graph *graph, struct dtl_node *node);

/**
 * Compacts the node heap, removing any node that is reachable from a merked root.  After calling
 * this, external references to nodes must be updated using `dtl_graph_gc_remap`.
 * Clears the set of marked roots.
 */
void
dtl_graph_gc_collect(struct dtl_graph *graph);

/**
 * Takes a node pointer from the previous generation and maps it to its new location post-collection
 * Returns NULL if the node has been collected.
 * Behaviour is undefined if node is not from previous generation.
 */
struct dtl_node *
dtl_graph_gc_remap(struct dtl_graph *graph, struct dtl_node *node);


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
dtl_graph_node_get_index(struct dtl_graph *graph, struct dtl_node *node);


