#include <check.h>
#include <stdlib.h>
#include <limits.h>
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
 * Test 1: Inserting a single vertex with negative integer payload
 * ============================================================ */

START_TEST(test_insert_negative)
{
    // Allocate and initialize a single integer
    int *value = malloc(sizeof(int));
    *value = -42;

    // Insert the vertex and verify success
    ck_assert_int_eq(graph_insert(g, value), GRAPH_STATUS_OK);
    
    // Verify the graph now has size 1
    ck_assert_int_eq(graph_size(g), 1);
}
END_TEST

/* ============================================================
 * Test 2: Inserting two vertices with a duplicate payload of 0
 * ============================================================ */

START_TEST(test_insert_duplicate_payload_zero)
{
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 0;
    *b = 0;
    
    // Insert the first vertex
    graph_insert(g, a);

    int dub = graph_insert(g, b);
    if (dub != GRAPH_STATUS_DUPLICATE_PAYLOAD) {
        free(b); // Free b only if it was not inserted
    }

    // Attempt to insert a duplicate payload
    ck_assert_int_eq(dub, GRAPH_STATUS_DUPLICATE_PAYLOAD);
    
    // Verify the graph still has size 1
    ck_assert_int_eq(graph_size(g), 1);
}
END_TEST

/* ============================================================
 * Test 3: Inserting two vertices with a duplicate payload of 42
 * ============================================================ */
START_TEST(test_insert_duplicate_payload)
{
    // Allocate and initialize a single integer
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 42;
    *b = 42;
    
    // Insert the first vertex
    graph_insert(g, a);

    int dub = graph_insert(g, b);
    if (dub != GRAPH_STATUS_DUPLICATE_PAYLOAD) {
        free(b); // Free b only if it was not inserted
    }

    // Attempt to insert a duplicate payload
    ck_assert_int_eq(dub, GRAPH_STATUS_DUPLICATE_PAYLOAD);
    
    // Verify the graph still has size 1
    ck_assert_int_eq(graph_size(g), 1);
}
END_TEST

/* ============================================================
 * Test 4: Inserting a single vertex with NULL payload
 * ============================================================ */

START_TEST(test_insert_null)
{
    // Allocate and initialize a single integer
    int *value = NULL;

    // Insert the vertex and verify NULL argument error
    ck_assert_int_eq(graph_insert(g, value), GRAPH_STATUS_NULL_ARGUMENT);
    
    // Verify the graph has size 0
    ck_assert_int_eq(graph_size(g), 0);
}
END_TEST

/* ============================================================
 * Test 4: Inserting a NULL graph
 * ============================================================ */

START_TEST(test_insert_null_graph)
{
    int *a = malloc(sizeof(int));
    *a = 1;

    ck_assert_int_eq(graph_insert(NULL, a), GRAPH_STATUS_NULL_ARGUMENT);
    
    ck_assert_int_eq(graph_size(g), 0);
}
END_TEST

/* ============================================================
 * Test 5: Inserting 3 vertices
 * ============================================================ */

START_TEST(test_insert_three_vertices)
{
    // Allocate and initialize a single integer
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    *c = 3;

    ck_assert_int_eq(graph_insert(g, a), GRAPH_STATUS_OK);
    ck_assert_int_eq(graph_insert(g, b), GRAPH_STATUS_OK);
    ck_assert_int_eq(graph_insert(g, c), GRAPH_STATUS_OK);
    
    // Verify the graph still has size 1
    ck_assert_int_eq(graph_size(g), 3);
}
END_TEST

/* ============================================================
 * Test 6: Inserting a single vertex with INT_MAX payload
 * ============================================================ */

START_TEST(test_insert_int_max)
{
    // Allocate and initialize a single integer
    int *value = malloc(sizeof(int));
    *value = INT_MAX;

    // Insert the vertex and verify success
    ck_assert_int_eq(graph_insert(g, value), GRAPH_STATUS_OK);
    
    // Verify the graph now has size 1
    ck_assert_int_eq(graph_size(g), 1);
}
END_TEST

/* ============================================================
 * Test 7: Creating graph with NULL compare_fn
 * ============================================================ */
START_TEST(test_create_null_compare_fn)
{
    Graph *null_graph = graph_create(NULL, destroy_int);
    
    // return NULL
    ck_assert_ptr_null(null_graph);
}
END_TEST

/* ============================================================
 * Test 8: Creating graph with NULL destroy_fn (valid case)
 * ============================================================ */
START_TEST(test_create_null_destroy_fn)
{
    Graph *no_destroy_graph = graph_create(compare_ints, NULL);
    
    // caller retains ownership
    ck_assert_ptr_nonnull(no_destroy_graph);
    
    int *value = malloc(sizeof(int));
    *value = 42;
    ck_assert_int_eq(graph_insert(no_destroy_graph, value), GRAPH_STATUS_OK);
    ck_assert_int_eq(graph_size(no_destroy_graph), 1);
    
    // destroy graph but won't free the payload
    graph_destroy(no_destroy_graph);
    
    // we retain ownership
    free(value);
}
END_TEST

/* ============================================================
 * Test 9: Destroying a NULL graph
 * ============================================================ */
START_TEST(test_destroy_null_graph)
{
    graph_destroy(NULL);
    
    //test passed w/ no crash
    ck_assert(1);
}
END_TEST

/* ============================================================
 * Test 10: Destroying graph with NULL destroy_fn
 * ============================================================ */
START_TEST(test_destroy_null_destroy_fn)
{
    Graph *graph = graph_create(compare_ints, NULL);
    
    ck_assert_ptr_nonnull(graph);

    graph_destroy(graph);
}
END_TEST


/* ============================================================
 * Suite Creation Function
 * ============================================================ */

Suite *insertion_suite(void)
{
    // Create a new test suite with a descriptive name
    Suite *s = suite_create("GraphInsertion");
    
    // Create a test case to group related tests
    TCase *tc = tcase_create("Basics");

    // Attach setup and teardown functions to run before/after each test
    // setup_graph() runs before each test to create a fresh graph
    // teardown_graph() runs after each test to clean up the graph
    tcase_add_checked_fixture(tc, setup_graph, teardown_graph);

    // Add individual test functions to the test case
    tcase_add_test(tc, test_insert_negative);
    tcase_add_test(tc, test_insert_duplicate_payload_zero);
    tcase_add_test(tc, test_insert_duplicate_payload);
    tcase_add_test(tc, test_insert_null);
    tcase_add_test(tc, test_insert_null_graph);
    tcase_add_test(tc, test_insert_three_vertices);
    tcase_add_test(tc, test_insert_int_max);
    tcase_add_test(tc, test_create_null_compare_fn);
    tcase_add_test(tc, test_create_null_destroy_fn);
    tcase_add_test(tc, test_destroy_null_graph);
    tcase_add_test(tc, test_destroy_null_destroy_fn);
    

    // Add the test case to the suite
    suite_add_tcase(s, tc);
    
    // Return the complete suite so main() can run it
    return s;
}