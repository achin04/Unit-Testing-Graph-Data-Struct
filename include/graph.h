#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stddef.h>

/* ============================================================
 * Function Pointers for managing the graph payload
 * ============================================================ */

/* Compare two payloads. 
 * Return 0 if equal, <0 if a is less than b or >0 otherwise.
 */
typedef int  (*GraphCompareFn)(const void *a, const void *b);

/* Destroy a payload stored in the graph.
 * Called once for each payload if destroy_fn != NULL.
 */
typedef void (*GraphDestroyFn)(void *payload);

/* ============================================================
 * Error Codes
 *
 * Operations that can fail return this status so callers can differentiate
 * between OK and specific failure modes (e.g., duplicates, missing nodes).
 * ============================================================ */
typedef enum GraphStatus {
    GRAPH_STATUS_OK = 0,
    GRAPH_STATUS_NULL_ARGUMENT,
    GRAPH_STATUS_DUPLICATE_PAYLOAD,
    GRAPH_STATUS_DUPLICATE_EDGE,
    GRAPH_STATUS_NOT_FOUND,
    GRAPH_STATUS_NO_MEMORY,
    GRAPH_STATUS_INVALID_OPERATION
} GraphStatus;

/* ============================================================
 * Opaque Type
 * ============================================================ */
typedef struct Graph Graph;

/* ============================================================
 * Graph Creation & Destruction
 * ============================================================ */

/*
 * Create an empty graph.
 *
 * compare_fn:
 *   Used to match payloads when adding edges or searching.
 *   Must not be NULL.
 *
 * destroy_fn:
 *   Used to free payloads when the graph is destroyed.
 *   May be NULL if the caller retains ownership.
 */
Graph *graph_create(GraphCompareFn compare_fn,
                    GraphDestroyFn destroy_fn);

/*
 * Destroy the graph.
 *
 * If destroy_fn != NULL:
 *   - destroy_fn(payload) is called on each payload.
 *
 * All graph memory is freed.
 */
void graph_destroy(Graph *g);

/* ============================================================
 * Payload Insertion
 * ============================================================ */

/*
 * Insert a payload into the graph.
 *
 * Duplicates are not permitted: the graph enforces unique payloads based on
 * the caller-supplied `compare_fn`.
 *
 * Preconditions:
 *   - g != NULL
 *   - payload != NULL
 *
 * Postconditions:
 *   - A new internal node is created to store the payload.
 *   - Returns `GRAPH_STATUS_OK` on success.
 *   - Returns `GRAPH_STATUS_DUPLICATE_PAYLOAD` when the payload already
 *     exists (duplicates are not permitted).
 *   - Returns `GRAPH_STATUS_NO_MEMORY` on allocation failure.
 *   - Returns `GRAPH_STATUS_NULL_ARGUMENT` when `g` or `payload` is NULL.
 */
GraphStatus graph_insert(Graph *g, void *payload);

/* ============================================================
 * Edge Creation Between Payloads
 * ============================================================ */

/*
 * Add a directed edge from payload 'from' to payload 'to'.
 *
 * Preconditions:
 *   - g != NULL
 *   - 'from' and 'to' must already exist in the graph
 *
 * Postconditions:
 *   - A directed connection is added between the two payloads.
 *   - Directed edges are unique; duplicates are rejected.
 *   - Returns `GRAPH_STATUS_OK` on success.
 *   - Returns `GRAPH_STATUS_NOT_FOUND` if either endpoint is missing.
 *   - Returns `GRAPH_STATUS_DUPLICATE_EDGE` when the directed edge already exists.
 *   - Returns `GRAPH_STATUS_NULL_ARGUMENT` when `g`, `from`, or `to` is NULL.
 */
GraphStatus graph_connect(Graph *g, const void *from, const void *to);

/* ============================================================
 * Querying Neighbors
 * ============================================================ */

/*
 * Retrieve neighbors of a payload.
 *
 * Preconditions:
 *   - g != NULL
 *   - payload exists in the graph
 *   - neighbors_out != NULL
 *   - count_out != NULL
 *
 * Postconditions:
 *   - *neighbors_out receives an array of payload pointers for the neighbours. 
 *     This variable tipe is a bit hard to read. You can translate it as:
 *     "pointer to an array of void pointers" 
 *     - "pointer to an array" is represented by `const **`
 *     - "of void pointers" is represented by `const void *`
 *     - pointers are `const` to keep the caller from accidentally modifying graph contents
 *     
 *     Note that this array is not dynamically allocated inside `graph_get_neighbors`, so it
 *     does not need to be freed by the caller.
 * 
 *   - *count_out receives the number of outgoing neighbors.
 *   - Returned neighbors are ordered in the order the edges were inserted.
 *   - Arrays remain valid until the graph is modified.
 *   - Returns `GRAPH_STATUS_OK` on success.
 *   - Returns `GRAPH_STATUS_NOT_FOUND` when `payload` is missing.
 *   - Returns `GRAPH_STATUS_NULL_ARGUMENT` when `g`, `payload`, or output
 *     params are NULL.
 */
GraphStatus graph_get_neighbors(const Graph *g,
                                const void *payload,
                                const void * const **neighbors_out,
                                int *count_out);

/* ============================================================
 * Graph Inspection Functions
 * 
 * This set of functions helps the user examine the graph contents
 * These functions have the precondition that all of their arguments are not NULL
 * These specific pre-conditions are omitted for brevity
 * ============================================================ */

/*
 * Get the number of payload nodes stored in the graph.
 *
 * Returns:
 *   the number of payload nodes stored in the graph if `g` is not NULL (may be 0)
 *   0 if `g` is NULL
 */
int graph_size(const Graph *g);

/*
 * Determine whether a payload exists in the graph.
 *
 * Returns:
 *   true  if exists
 *   false otherwise, of if any of the arguments are NULL
 */
bool graph_contains(const Graph *g, const void *payload);

/* Get the number of outgoing edges (outdegree) that originate from payload. 
 * 
 * Returns:
 *   the outdegree number if arguments are not NULL (may be 0)
 *   0 if any of the arguments are NULL
 */
int graph_outdegree(const Graph *g, const void *payload);

/* Check whether a directed edge exists from -> to. 
 * 
 * Returns:
 *   true  if exists
 *   false otherwise, of if any of the arguments are NULL
 */
bool graph_has_edge(const Graph *g, const void *from, const void *to);

/* Get the number of incoming edges (indegree) that end at the payload.
 * 
 * Returns:
 *   the indegree number if arguments are not NULL (may be 0)
 *   0 if any of the arguments are NULL
 */
int graph_indegree(const Graph *g, const void *payload);

/* Get the number of edges in the graph.
 * 
 * Returns:
 *   the number of edges (may be 0)
 *   0 if `g` is NULL
 */
int graph_edge_count(const Graph *g);

/* Get an array of all payloads for iteration/testing.
 * Payloads are reported in insertion order and remain valid until graph
 * modification.
 * Preconditions:
 *   - g != NULL
 *   - payload exists in the graph
 *   - payloads_out != NULL
 *   - count_out != NULL
 * 
 * Postconditions:
 * - *payloads_out receives an array of pointers to all the payloads in the graph
 *   This variable is a pointer to an array of void pointers, which is the same
 *   type as `neighbors_out` in `graph_get_neighbors`. See the comments in graph_get_neighbors
 *   to better understand the type. This array is also not dynamically allocated and does not
 *   need to be freed by the caller.
 * - *count_out receives the number of payloads and is the total number of nodes in the graph.
 * - Arrays remain valid until the graph is modified.
 * - Returns `GRAPH_STATUS_OK` on success
 * - Returns `GRAPH_STATUS_NULL_ARGUMENT` when any of the arguments are NULL.
 */
GraphStatus graph_get_all_payloads(const Graph *g,
                                   const void * const **payloads_out,
                                   int *count_out);

/* Check whether a payload is reachable from another.
 * 
 * Returns:
 *   true  if reachable
 *   false otherwise, of if any of the arguments are NULL
 */
bool graph_reachable(const Graph *g, const void *from, const void *to);

/* Check whether the graph contains a cycle
 * 
 * Returns:
 *   true  if a cycle exists
 *   false otherwise, or if `g` is NULL
 */
bool graph_has_cycle(const Graph *g);

/* Check whether the graph is fully connected
 * (all nodes reachable from first inserted payload).
 *
 * Returns:
 *   true  if graph is fully connected
 *   false otherwise, or if `g` is NULL
 */
bool graph_is_connected(const Graph *g);

/* ============================================================
 * Removal Helpers
 * ============================================================ */

/* Remove a directed edge between two payloads.
 * 
 * Preconditions:
 *   - g != NULL
 *   - from != NULL
 *   - to != NULL
 *
 * Postconditions:
 *   - The directed edge has been removed between the two payloads
 *   - Returns:
 *     - `GRAPH_STATUS_OK` when the edge was removed.
 *     - `GRAPH_STATUS_NOT_FOUND` when the edge or either payload is missing.
 *     - `GRAPH_STATUS_NULL_ARGUMENT` when `g`, `from`, or `to` is NULL.
 */
GraphStatus graph_disconnect(Graph *g, const void *from, const void *to);

/* Remove a payload and all relevant edges from the graph.
 *
 * Preconditions:
 *   - g != NULL
 *   - payload != NULL
 * 
 * Postconditions:
 *   - The payload was removed from the graph
 *   - The payload has been freed
 *   - All the edges leading to and from this payload have been removed
 *   - Returns:
 *     - `GRAPH_STATUS_OK` when the payload existed and was removed.
 *     - `GRAPH_STATUS_NOT_FOUND` when the payload is missing.
 *     - `GRAPH_STATUS_NULL_ARGUMENT` when `g` or `payload` is NULL.
 */
GraphStatus graph_remove(Graph *g, const void *payload);

#endif /* GRAPH_H */