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
 * Test 1: two vertices connection with two edges
 * ============================================================ */

START_TEST(test_connect_two_edges)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    // Insert both vertices into the graph
    graph_insert(g, a);
    graph_insert(g, b);

    // Connect a → b and verify success
    ck_assert_int_eq(graph_connect(g, a, b), GRAPH_STATUS_OK);
    ck_assert_int_eq(graph_connect(g, b, a), GRAPH_STATUS_OK);
    
    // Verify the edge exists
    ck_assert(graph_has_edge(g, a, b));
    ck_assert(graph_has_edge(g, b, a));
    }
    END_TEST

/* ============================================================
 * Test 2: two vertices connection with duplicate edges
 * ============================================================ */

START_TEST(test_connect_duplicate_edges)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    // Insert both vertices into the graph
    graph_insert(g, a);
    graph_insert(g, b);

    // Connect a → b and verify success
    ck_assert_int_eq(graph_connect(g, a, b), GRAPH_STATUS_OK);
    ck_assert_int_eq(graph_connect(g, a, b), GRAPH_STATUS_DUPLICATE_EDGE);
    
    // Verify the edge exists
    ck_assert(graph_has_edge(g, a, b));
    }
    END_TEST

/* ============================================================
 * Test 3: connection with missing edge
 * ============================================================ */

START_TEST(test_connect_missing_edge)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);

    // Connect a → b 
    ck_assert_int_eq(graph_connect(g, a, b), GRAPH_STATUS_NOT_FOUND);
    
    // Verify the edge exists
    ck_assert(!graph_has_edge(g, a, b));
    }
    END_TEST

/* ============================================================
 * Test 4: disconnecting an edge
 * ============================================================ */

START_TEST(test_disconnect_edge)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    // Connect a → b 
    ck_assert_int_eq(graph_connect(g, a, b), GRAPH_STATUS_OK);

    graph_disconnect(g, a, b);
    
    // Verify the edge exists
    ck_assert(!graph_has_edge(g, a, b));
    }
    END_TEST

/* ============================================================
 * Test 5: disconnecting an edge with missing payload
 * ============================================================ */

START_TEST(test_disconnect_edge_missing)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    ck_assert_int_eq(graph_disconnect(g, a, b), GRAPH_STATUS_NOT_FOUND);
    }
    END_TEST

/* ============================================================
 * Test 6: disconnecting an edge with NULL payload
 * ============================================================ */

START_TEST(test_disconnect_edge_null_from)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    graph_connect(g, a, b);

    ck_assert_int_eq(graph_disconnect(g, NULL, b), GRAPH_STATUS_NULL_ARGUMENT);
    }
    END_TEST

/* ============================================================
 * Test 6: disconnecting an edge with NULL from payload
 * ============================================================ */

START_TEST(test_disconnect_edge_null_to)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    graph_connect(g, a, b);

    ck_assert_int_eq(graph_disconnect(g, a, NULL), GRAPH_STATUS_NULL_ARGUMENT);
    }
    END_TEST

/* ============================================================
 * Test 6: disconnect with NULL graph
 * ============================================================ */

START_TEST(test_disconnect_null_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    graph_connect(g, a, b);

    ck_assert_int_eq(graph_disconnect(NULL, a, b), GRAPH_STATUS_NULL_ARGUMENT);
    }
    END_TEST
/* ============================================================
 * Test 7: connecting to a NULL edge 
 * ============================================================ */

START_TEST(test_connect_null_edge)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    b = NULL;

    // Connect a → b 
    ck_assert_int_eq(graph_connect(g, a, b), GRAPH_STATUS_NULL_ARGUMENT);
    
    // Verify the edge exists
    ck_assert(!graph_has_edge(g, a, b));
    }
    END_TEST

/* ============================================================
 * Test 7: connecting from a NULL edge
 * ============================================================ */

START_TEST(test_connect_null_edge_from)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    a = NULL;

    // Connect a → b 
    ck_assert_int_eq(graph_connect(g, a, b), GRAPH_STATUS_NULL_ARGUMENT);
    
    // Verify the edge exists
    ck_assert(!graph_has_edge(g, a, b));
    }
    END_TEST

/* ============================================================
 * Test 7: graph connect with both NULL edges
 * ============================================================ */

START_TEST(test_connect_both_null_edge)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);

    a = NULL;
    b = NULL;

    // Connect a → b 
    ck_assert_int_eq(graph_connect(g, a, b), GRAPH_STATUS_NULL_ARGUMENT);
    
    // Verify the edge exists
    ck_assert(!graph_has_edge(g, a, b));
    }
    END_TEST

/* ============================================================
 * Test 7: graph connect with NULL graph
 * ============================================================ */

START_TEST(test_connect_null_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);
    
    // Connect a → b 
    ck_assert_int_eq(graph_connect(NULL, a, b), GRAPH_STATUS_NULL_ARGUMENT);
    
    // Verify the edge exists
    ck_assert(!graph_has_edge(g, a, b));
    }
    END_TEST

/* ============================================================
 * Test 8: test size of large graph
 * ============================================================ */

START_TEST(test_large_graph_size)
{
    const int NUM_NODES = 1000;
    for (int i = 0; i < NUM_NODES; ++i) {
        int *val = malloc(sizeof(int));
        *val = i;
        ck_assert_int_eq(graph_insert(g, val), GRAPH_STATUS_OK);
    }
    ck_assert_int_eq(graph_size(g), NUM_NODES);
    }
    END_TEST

/* ============================================================
 * Test 7: graph has edge with NULL graph
 * ============================================================ */

START_TEST(test_has_edge_null_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);
    graph_connect(g, a, b);

    ck_assert_int_eq(graph_has_edge(NULL, a, b), 0);
    }
    END_TEST

/* ============================================================
 * Test 7: graph edge count with cycle
 * ============================================================ */ 
START_TEST(test_edge_count_with_cycle)
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
    
    ck_assert_int_eq(graph_edge_count(g), NUM_NODES);
    }
    END_TEST

/* ============================================================
 * Test 7:  graph edge count no edges
 * ============================================================ */

START_TEST(test_edge_count_no_edges)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);
    
    ck_assert_int_eq(graph_edge_count(g), 0);
    }
    END_TEST

/* ============================================================
 * Test 7:  graph edge count NULL graph
 * ============================================================ */

START_TEST(test_edge_count_null_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);
    
    ck_assert_int_eq(graph_edge_count(NULL), 0);
    }
    END_TEST

/* ============================================================
 * Test 7: self loop edge
 * ============================================================ */

START_TEST(test_self_loop_edge)
{
    int *a = malloc(sizeof(int));
    *a = 1;
    
    graph_insert(g, a);
    
    int status = graph_connect(g, a, a);
    
    if (status == GRAPH_STATUS_OK) {
        ck_assert(graph_has_edge(g, a, a));
        ck_assert_int_eq(graph_edge_count(g), 1);
        ck_assert_int_eq(graph_outdegree(g, a), 1);
        ck_assert_int_eq(graph_indegree(g, a), 0);
    }
    }
    END_TEST

/* ============================================================
 * Test: disconnect edge and verify return value
 * ============================================================ */
START_TEST(test_disconnect_edge_return_ok)
{
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);
    graph_connect(g, a, b);
    
    ck_assert_int_eq(graph_disconnect(g, a, b), GRAPH_STATUS_OK);
    ck_assert(!graph_has_edge(g, a, b));
    ck_assert_int_eq(graph_edge_count(g), 0);
}
END_TEST

/* ============================================================
 * Test: remove node that has incoming edges
 * ============================================================ */
START_TEST(test_remove_with_incoming_edges)
{
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    *c = 3;
    
    graph_insert(g, a);
    graph_insert(g, b);
    graph_insert(g, c);
    
    graph_connect(g, a, b);
    graph_connect(g, c, b);
    
    ck_assert_int_eq(graph_remove(g, b), GRAPH_STATUS_OK);
    ck_assert_int_eq(graph_size(g), 2);
    ck_assert_int_eq(graph_edge_count(g), 0);
    ck_assert(!graph_contains(g, b));
}
END_TEST

/* ============================================================
 * Test: remove node and verify edges are cleaned up
 * ============================================================ */
START_TEST(test_remove_cleans_edges)
{
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    
    graph_insert(g, a);
    graph_insert(g, b);
    graph_connect(g, a, b);
    graph_connect(g, b, a);
    
    ck_assert_int_eq(graph_edge_count(g), 2);
    ck_assert_int_eq(graph_remove(g, a), GRAPH_STATUS_OK);
    ck_assert_int_eq(graph_edge_count(g), 0);
    ck_assert_int_eq(graph_outdegree(g, b), 0);
    ck_assert_int_eq(graph_indegree(g, b), 0);
}
END_TEST

/* ============================================================
 * Suite Creation Function
 * 
 * This function builds and returns a test suite for the Check framework.
 * A Suite is a collection of test cases that are run together.
 * ============================================================ */

Suite *edge_suite(void)
{
    // Create a new test suite with a descriptive name
    Suite *s = suite_create("GraphEdge");
    
    // Create a test case to group related tests
    TCase *tc = tcase_create("Basics");

    // Attach setup and teardown functions to run before/after each test
    // setup_graph() runs before each test to create a fresh graph
    // teardown_graph() runs after each test to clean up the graph
    tcase_add_checked_fixture(tc, setup_graph, teardown_graph);

    // Add individual test functions to the test case
    tcase_add_test(tc, test_connect_two_edges);
    tcase_add_test(tc, test_connect_duplicate_edges);
    tcase_add_test(tc, test_connect_missing_edge);
    tcase_add_test(tc, test_disconnect_edge);
    tcase_add_test(tc, test_disconnect_edge_missing);
    tcase_add_test(tc, test_disconnect_edge_null_from);
    tcase_add_test(tc, test_disconnect_edge_null_to);
    tcase_add_test(tc, test_disconnect_null_graph);
    tcase_add_test(tc, test_connect_null_edge);
    tcase_add_test(tc, test_connect_null_edge_from);
    tcase_add_test(tc, test_connect_both_null_edge);
    tcase_add_test(tc, test_connect_null_graph);
    tcase_add_test(tc, test_large_graph_size);
    tcase_add_test(tc, test_has_edge_null_graph);
    tcase_add_test(tc, test_edge_count_with_cycle);
    tcase_add_test(tc, test_edge_count_no_edges);
    tcase_add_test(tc, test_edge_count_null_graph);
    tcase_add_test(tc, test_self_loop_edge);
    tcase_add_test(tc, test_disconnect_edge_return_ok);
    tcase_add_test(tc, test_remove_with_incoming_edges);
    tcase_add_test(tc, test_remove_cleans_edges);

    // Add the test case to the suite
    suite_add_tcase(s, tc);
    
    // Return the complete suite so main() can run it
    return s;
}