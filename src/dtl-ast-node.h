#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "dtl-location.h"

struct dtl_ast_node;

/* === Generic ================================================================================== */

int
dtl_ast_node_get_tag(struct dtl_ast_node *);

void
dtl_ast_node_update_bounds(struct dtl_ast_node *, struct dtl_location start, struct dtl_location end);

struct dtl_location
dtl_ast_node_get_start(struct dtl_ast_node *);

struct dtl_location
dtl_ast_node_get_end(struct dtl_ast_node *);

void
dtl_ast_node_destroy(struct dtl_ast_node *node);

/* === Type Specific ============================================================================ */

/* --- Empty Nodes ------------------------------------------------------------------------------ */

struct dtl_ast_node *
dtl_ast_node_create_empty(int tag);

/* --- Container Nodes -------------------------------------------------------------------------- */

struct dtl_ast_node *
dtl_ast_node_create_with_children(int tag);

struct dtl_ast_node *
dtl_ast_node_append_child(struct dtl_ast_node *node, struct dtl_ast_node *child);

bool
dtl_ast_node_has_children(struct dtl_ast_node *);

size_t
dtl_ast_node_get_num_children(struct dtl_ast_node *node);

struct dtl_ast_node *
dtl_ast_node_get_child(struct dtl_ast_node *node, size_t index);

/* --- Data Nodes ------------------------------------------------------------------------------- */

struct dtl_ast_node *
dtl_ast_node_create_with_data(int tag, size_t size);

bool
dtl_ast_node_has_data(struct dtl_ast_node *);

size_t
dtl_ast_node_get_data_size(struct dtl_ast_node *);

void *
dtl_ast_node_get_data(struct dtl_ast_node *);
