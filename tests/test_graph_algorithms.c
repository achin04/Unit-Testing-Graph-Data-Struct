#include <check.h>
#include <stdlib.h>
#include "graph.h"

/* ============================================================
 * Integer compare/destroy callbacks for initializing the graph
 * ============================================================ */

static int compare_ints(const void *a, const void *b) {
    const int *ia = a;
    const int *ib = b;
    if (*ia < *ib) return -1;
    if (*ia > *ib) return 1;
    return 0;
}

static void destroy_int(void *p) {
    free(p);
}

/* ============================================================
 * Setup and Teardown fixtures
 * ============================================================ */

static Graph *g = NULL;

static void setup_graph(void)
{
    g = graph_create(compare_ints, destroy_int);
    ck_assert_ptr_nonnull(g);
}

static void teardown_graph(void)
{
    /* graph_destroy must call destroy_int on all payloads */
    graph_destroy(g);
    g = NULL;
}

/* ============================================================
 * Test 1: graph is fully connected
 * ============================================================ */

START_TEST(test_fully_connected)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        graph_insert(g, nodes[i]);
    }
    // Connect all nodes to the first node
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        graph_connect(g, nodes[0], nodes[i + 1]);
    }
    
    ck_assert(graph_is_connected(g));
    }
    END_TEST

/* ============================================================
 * Test 2: graph is NOT fully connected
 * ============================================================ */

START_TEST(test_not_fully_connected)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        graph_insert(g, nodes[i]);
    }
    // Connect all nodes to the first node
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        if (i == 5) continue; // Skip one connection to break connectivity
        graph_connect(g, nodes[0], nodes[i + 1]);
    }

    ck_assert(!graph_is_connected(g));
    }
    END_TEST

START_TEST(test_fully_connected_null_graph)
{
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    graph_connect(g, a, b);

    ck_assert(!graph_is_connected(NULL));
    }
END_TEST

/* ============================================================
 * Test 3: payload is reachable from another payload
 * ============================================================ */

START_TEST(test_payload_reachable)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        graph_insert(g, nodes[i]);
    }
    // Connect all nodes to the first node
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        graph_connect(g, nodes[0], nodes[i + 1]);
    }

    ck_assert(graph_reachable(g, nodes[0], nodes[NUM_NODES - 1]));
    }
    END_TEST

/* ============================================================
 * Test 4: payload is NOT reachable from another payload
 * ============================================================ */

START_TEST(test_payload_not_reachable)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        graph_insert(g, nodes[i]);
    }
    // Connect all nodes to the first node
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        graph_connect(g, nodes[0], nodes[i + 1]);
    }
    
    ck_assert(!graph_reachable(g, nodes[1], nodes[2]));
    }
    END_TEST

START_TEST(test_payload_null_from_reachable)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        graph_insert(g, nodes[i]);
    }
    // Connect all nodes to the first node
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        graph_connect(g, nodes[0], nodes[i + 1]);
    }
    
    ck_assert(!graph_reachable(g, NULL, nodes[2]));
    }
    END_TEST

START_TEST(test_payload_null_to_reachable)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        graph_insert(g, nodes[i]);
    }
    // Connect all nodes to the first node
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        graph_connect(g, nodes[0], nodes[i + 1]);
    }
    
    ck_assert(!graph_reachable(g, nodes[1], NULL));
    }
    END_TEST

START_TEST(test_payload_null_graph)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        graph_insert(g, nodes[i]);
    }
    // Connect all nodes to the first node
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        graph_connect(g, nodes[0], nodes[i + 1]);
    }
    
    ck_assert(!graph_reachable(NULL, nodes[1], nodes[2]));
    }
    END_TEST

/* ============================================================
 * Test 5: entire graph is a cycle
 * ============================================================ */

START_TEST(test_has_cycle)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        graph_insert(g, nodes[i]);
    }
    
    // Connect nodes in a chain
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        graph_connect(g, nodes[i], nodes[i + 1]);
    }
    
    // Create cycle by connecting last node back to first
    graph_connect(g, nodes[NUM_NODES - 1], nodes[0]);
    
    ck_assert(graph_has_cycle(g));
    }
    END_TEST

/* ============================================================
 * Test 6: entire graph is NOT a cycle
 * ============================================================ */

START_TEST(test_not_cycle)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    graph_connect(g, a, b);
    
    ck_assert(!graph_has_cycle(g));
    }
    END_TEST

START_TEST(test_null_cycle)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    graph_connect(g, a, b);
    
    ck_assert(!graph_has_cycle(NULL));
    }
    END_TEST

/* ============================================================
 * Test: single node is connected
 * ============================================================ */
START_TEST(test_single_node_connected)
{
    int *a = malloc(sizeof(int));
    *a = 1;
    graph_insert(g, a);
    
    ck_assert(graph_is_connected(g));
}
END_TEST

/* ============================================================
 * Test: self reachability
 * ============================================================ */
START_TEST(test_self_reachable)
{
    int *a = malloc(sizeof(int));
    *a = 1;
    graph_insert(g, a);
    
    // a node is reachable from itself
    ck_assert(graph_reachable(g, a, a));
}
END_TEST

/* ============================================================
 * Test: reachability with missing payload
 * ============================================================ */
START_TEST(test_reachable_not_found)
{
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    
    ck_assert(!graph_reachable(g, a, b));
    ck_assert(!graph_reachable(g, b, a));
}
END_TEST

/* ============================================================
 * Test: cycle detection with self loop
 * ============================================================ */
START_TEST(test_self_loop_cycle)
{
    int *a = malloc(sizeof(int));
    *a = 1;
    
    graph_insert(g, a);
    graph_connect(g, a, a);  // self loop
    
    ck_assert(graph_has_cycle(g));
}
END_TEST

/* ============================================================
 * Test: empty graph has no cycle
 * ============================================================ */
START_TEST(test_empty_graph_no_cycle)
{
    ck_assert(!graph_has_cycle(g));
}
END_TEST

/* ============================================================
 * Test: empty graph is_connected
 * ============================================================ */
START_TEST(test_empty_graph_connected)
{
    // no nodes are unreachable in an empty graph (true)
    ck_assert(graph_is_connected(g));
}
END_TEST

/* ============================================================
 * Suite Creation Function
 * 
 * This function builds and returns a test suite for the Check framework.
 * A Suite is a collection of test cases that are run together.
 * ============================================================ */

Suite *algorithms_suite(void)
{
    // Create a new test suite with a descriptive name
    Suite *s = suite_create("GraphAlgorithms");
    
    // Create a test case to group related tests
    TCase *tc = tcase_create("Basics");

    // Attach setup and teardown functions to run before/after each test
    // setup_graph() runs before each test to create a fresh graph
    // teardown_graph() runs after each test to clean up the graph
    tcase_add_checked_fixture(tc, setup_graph, teardown_graph);

    // Add individual test functions to the test case
    tcase_add_test(tc, test_fully_connected);
    tcase_add_test(tc, test_not_fully_connected);
    tcase_add_test(tc, test_fully_connected_null_graph);
    tcase_add_test(tc, test_payload_reachable);
    tcase_add_test(tc, test_payload_not_reachable);
    tcase_add_test(tc, test_payload_null_from_reachable);
    tcase_add_test(tc, test_payload_null_to_reachable);
    tcase_add_test(tc, test_payload_null_graph);
    tcase_add_test(tc, test_has_cycle);
    tcase_add_test(tc, test_not_cycle);
    tcase_add_test(tc, test_null_cycle);
    tcase_add_test(tc, test_single_node_connected);
    tcase_add_test(tc, test_self_reachable);
    tcase_add_test(tc, test_reachable_not_found);
    tcase_add_test(tc, test_self_loop_cycle);
    tcase_add_test(tc, test_empty_graph_no_cycle);
    tcase_add_test(tc, test_empty_graph_connected);

    // Add the test case to the suite
    suite_add_tcase(s, tc);
    
    // Return the complete suite so main() can run it
    return s;
}