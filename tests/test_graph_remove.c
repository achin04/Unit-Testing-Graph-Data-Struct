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
 * Test 1: graph remove payload success
 * ============================================================ */

START_TEST(test_remove_payload)
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

    ck_assert_int_eq(graph_remove(g, nodes[0]), GRAPH_STATUS_OK);
    ck_assert(!graph_is_connected(g));
    ck_assert_int_eq(graph_edge_count(g), 0);
    }
    END_TEST

/* ============================================================
 * Test 2: graph remove payload NOT FOUND
 * ============================================================ */

START_TEST(test_remove_payload_not_found)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        if (i == 5) continue; // Skip inserting node 5
        graph_insert(g, nodes[i]);
    }
    // Connect all nodes to the first node
    for (int i = 0; i < NUM_NODES - 1; ++i) {
        if (i == 5) continue; // Skip connection for missing node
        graph_connect(g, nodes[0], nodes[i + 1]);
    }
    ck_assert_int_eq(graph_remove(g, nodes[5]), GRAPH_STATUS_NOT_FOUND);
    }
    END_TEST

/* ============================================================
 * Test 3: graph remove payload NULL ARGUMENT
 * ============================================================ */

START_TEST(test_remove_payload_null_argument)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        if (i == 5) {
            nodes[i] = NULL;
        }
        else {
            nodes[i] = malloc(sizeof(int));
            *nodes[i] = i;
        }
        graph_insert(g, nodes[i]);
    }

    ck_assert_int_eq(graph_remove(g, nodes[5]), GRAPH_STATUS_NULL_ARGUMENT);
    }
    END_TEST

/* ============================================================
 * Test 4: graph remove payload with NULL graph
 * ============================================================ */

START_TEST(test_remove_payload_null_graph)
{
    const int NUM_NODES = 10;
    int *nodes[NUM_NODES];

    // Create and insert all nodes
    for (int i = 0; i < NUM_NODES; ++i) {
        nodes[i] = malloc(sizeof(int));
        *nodes[i] = i;
        graph_insert(g, nodes[i]);
    }

    ck_assert_int_eq(graph_remove(NULL, nodes[0]), GRAPH_STATUS_NULL_ARGUMENT);
    }
END_TEST

/* ============================================================
 * Suite Creation Function
 * 
 * This function builds and returns a test suite for the Check framework.
 * A Suite is a collection of test cases that are run together.
 * ============================================================ */

Suite *remove_suite(void)
{
    // Create a new test suite with a descriptive name
    Suite *s = suite_create("GraphRemove");
    
    // Create a test case to group related tests
    TCase *tc = tcase_create("Basics");

    // Attach setup and teardown functions to run before/after each test
    // setup_graph() runs before each test to create a fresh graph
    // teardown_graph() runs after each test to clean up the graph
    tcase_add_checked_fixture(tc, setup_graph, teardown_graph);

    // Add individual test functions to the test case
    tcase_add_test(tc, test_remove_payload);
    tcase_add_test(tc, test_remove_payload_not_found);
    tcase_add_test(tc, test_remove_payload_null_argument);
    tcase_add_test(tc, test_remove_payload_null_graph);

    // Add the test case to the suite
    suite_add_tcase(s, tc);
    
    // Return the complete suite so main() can run it
    return s;
}