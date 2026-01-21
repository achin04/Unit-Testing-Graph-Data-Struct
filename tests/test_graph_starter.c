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
 * Test 1: Inserting a single vertex
 * Tests that graph_insert successfully adds one vertex
 * ============================================================ */

START_TEST(test_insert_single_vertex)
{
    // Allocate and initialize a single integer
    int *value = malloc(sizeof(int));
    *value = 42;

    // Insert the vertex and verify success
    ck_assert_int_eq(graph_insert(g, value), GRAPH_STATUS_OK);
    
    // Verify the graph now has size 1
    ck_assert_int_eq(graph_size(g), 1);
}
END_TEST

/* ============================================================
 * Test 2: Connecting two vertices
 * Tests that graph_connect creates an edge between two vertices
 * ============================================================ */

START_TEST(test_connect_two_vertices)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 10;
    *b = 20;
    
    // Insert both vertices into the graph
    graph_insert(g, a);
    graph_insert(g, b);

    // Connect a → b and verify success
    ck_assert_int_eq(graph_connect(g, a, b), GRAPH_STATUS_OK);
    
    // Verify the edge exists
    ck_assert(graph_has_edge(g, a, b));
}
END_TEST



/* ============================================================
 * Suite Creation Function
 * 
 * This function builds and returns a test suite for the Check framework.
 * A Suite is a collection of test cases that are run together.
 * ============================================================ */

Suite *starter_suite(void)
{
    // Create a new test suite with a descriptive name
    Suite *s = suite_create("GraphStarter");
    
    // Create a test case to group related tests
    TCase *tc = tcase_create("Basics");

    // Attach setup and teardown functions to run before/after each test
    // setup_graph() runs before each test to create a fresh graph
    // teardown_graph() runs after each test to clean up the graph
    tcase_add_checked_fixture(tc, setup_graph, teardown_graph);

    // Add individual test functions to the test case
    tcase_add_test(tc, test_insert_single_vertex);
    tcase_add_test(tc, test_connect_two_vertices);

    // Add the test case to the suite
    suite_add_tcase(s, tc);
    
    // Return the complete suite so main() can run it
    return s;
}