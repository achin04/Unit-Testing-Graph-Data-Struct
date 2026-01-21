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
 * Test 1: get neighbors of small graph
 * ============================================================ */

START_TEST(test_neighbors_small_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    *c = 3;

    graph_insert(g, a);
    graph_insert(g, b);
    graph_insert(g, c);

    // Connect a → b and a → c
    graph_connect(g, a, b);
    graph_connect(g, a, c);

    // Query neighbors of a
    const void * const *neighbors = NULL;
    int count = 0;
    
    // Assert the call succeeded
    ck_assert_int_eq(graph_get_neighbors(g, a, &neighbors, &count), GRAPH_STATUS_OK);
    
    // Assert we got 2 neighbors
    ck_assert_int_eq(count, 2);
    
    // Assert the neighbors are b and c (in insertion order)
    ck_assert_ptr_eq(neighbors[0], b);
    ck_assert_ptr_eq(neighbors[1], c);
    }
    END_TEST

/* ============================================================
 * Test 2: get neighbors of NULL graph
 * ============================================================ */

START_TEST(test_neighbors_null_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;

    graph_insert(g, a);
    graph_insert(g, b);

    // Connect a → b 
    graph_connect(g, a, b);

    // Query neighbors of a
    const void * const *neighbors = NULL;
    int count = 0;
    
    ck_assert_int_eq(graph_get_neighbors(NULL, a, &neighbors, &count), GRAPH_STATUS_NULL_ARGUMENT);
    }
    END_TEST

/* ============================================================
 * Test 3: get neighbors of NULL payload
 * ============================================================ */

START_TEST(test_neighbors_null_payload)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;

    graph_insert(g, a);
    graph_insert(g, b);

    // Connect a → b 
    graph_connect(g, a, b);

    // Query neighbors of a
    const void * const *neighbors = NULL;
    int count = 0;
    
    ck_assert_int_eq(graph_get_neighbors(g, NULL, &neighbors, &count), GRAPH_STATUS_NULL_ARGUMENT);
    }
    END_TEST

/* ============================================================
 * Test 4: get neighbors of NULL neighbors_out
 * ============================================================ */

START_TEST(test_neighbors_null_neighbors_out)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;

    graph_insert(g, a);
    graph_insert(g, b);

    // Connect a → b 
    graph_connect(g, a, b);

    // Query neighbors of a
    int count = 0;
    
    ck_assert_int_eq(graph_get_neighbors(g, a, NULL, &count), GRAPH_STATUS_NULL_ARGUMENT);
    }
    END_TEST

/* ============================================================
 * Test 5: get neighbors of NULL payload
 * ============================================================ */

START_TEST(test_neighbors_null_count_out)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;

    graph_insert(g, a);
    graph_insert(g, b);

    // Connect a → b 
    graph_connect(g, a, b);

    // Query neighbors of a
    const void * const *neighbors = NULL;
    
    ck_assert_int_eq(graph_get_neighbors(g, a, &neighbors, NULL), GRAPH_STATUS_NULL_ARGUMENT);
    }
    END_TEST

/* ============================================================
 * Test 6: get neighbors missing payload
 * ============================================================ */

START_TEST(test_neighbors_missing_payload)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;

    graph_insert(g, a);

    const void * const *neighbors = NULL;
    int count = 0;

    ck_assert_int_eq(graph_get_neighbors(g, b, &neighbors, &count), GRAPH_STATUS_NOT_FOUND);
    }
    END_TEST

/* ============================================================
 * Test: neighbors of node with no edges
 * ============================================================ */
START_TEST(test_neighbors_no_edges)
{
    int *a = malloc(sizeof(int));
    *a = 1;
    
    graph_insert(g, a);
    
    const void * const *neighbors = NULL;
    int count = 0;
    
    ck_assert_int_eq(graph_get_neighbors(g, a, &neighbors, &count), GRAPH_STATUS_OK);
    ck_assert_int_eq(count, 0);
}
END_TEST

/* ============================================================
 * Test: empty graph get_all_payloads
 * ============================================================ */
START_TEST(test_get_all_payloads_empty)
{
    const void * const *payloads = NULL;
    int count = 0;
    
    ck_assert_int_eq(graph_get_all_payloads(g, &payloads, &count), GRAPH_STATUS_OK);
    ck_assert_int_eq(count, 0);
}
END_TEST

/* ============================================================
 * Test 7: get all payloads 
 * ============================================================ */

START_TEST(test_get_all_payloads)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    *c = 3;

    graph_insert(g, a);
    graph_insert(g, b);
    graph_insert(g, c);

    const void * const *payloads = NULL;
    int count = 0;

    ck_assert_int_eq(graph_get_all_payloads(g, &payloads, &count), GRAPH_STATUS_OK);

    ck_assert_int_eq(count, 3);

    ck_assert_ptr_eq(payloads[0], a);
    ck_assert_ptr_eq(payloads[1], b);
    ck_assert_ptr_eq(payloads[2], c);

    ck_assert(graph_contains(g, a));
    ck_assert(graph_contains(g, b));
    ck_assert(graph_contains(g, c));
    }
    END_TEST

/* ============================================================
 * Test 8: get all payloads with NULL graph
 * ============================================================ */

START_TEST(test_null_get_all_payloads_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    *c = 3;

    graph_insert(g, a);
    graph_insert(g, b);
    graph_insert(g, c);

    const void * const *payloads = NULL;
    int count = 0;

    ck_assert_int_eq(graph_get_all_payloads(NULL, &payloads, &count), GRAPH_STATUS_NULL_ARGUMENT);

    }
    END_TEST

/* ============================================================
 * Test 8: get all payloads with NULL count_out
 * ============================================================ */

START_TEST(test_null_get_all_payloads_out)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    *c = 3;

    graph_insert(g, a);
    graph_insert(g, b);
    graph_insert(g, c);

    const void * const *payloads = NULL;

    ck_assert_int_eq(graph_get_all_payloads(g, &payloads, NULL), GRAPH_STATUS_NULL_ARGUMENT);

    }
    END_TEST

/* ============================================================
 * Test 9: get all payloads with NULL payloads_out
 * ============================================================ */

START_TEST(test_null_get_all_payloads_count_out)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    *c = 3;

    graph_insert(g, a);
    graph_insert(g, b);
    graph_insert(g, c);

    int count = 0;

    ck_assert_int_eq(graph_get_all_payloads(g, NULL, &count), GRAPH_STATUS_NULL_ARGUMENT);

    }
    END_TEST

/* ============================================================
 * Test 10: get outdegree with correct status
 * ============================================================ */

START_TEST(test_outdegree_status)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    *c = 3;

    graph_insert(g, a);
    graph_insert(g, b);
    graph_insert(g, c);

    // Connect a → b 
    graph_connect(g, a, b);
    graph_connect(g, a, c);

    ck_assert_int_eq(graph_outdegree(g, a), 2);

    }
    END_TEST

/* ============================================================
 * Test 10: get outdegree with 0
 * ============================================================ */

START_TEST(test_outdegree_with_zero)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    *a = 1;

    ck_assert_int_eq(graph_outdegree(g, a), 0);

    }
    END_TEST

/* ============================================================
 * Test 10: get outdegree with NULL
 * ============================================================ */

START_TEST(test_outdegree_with_NULL)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    *a = 1;

    ck_assert_int_eq(graph_outdegree(NULL, a), 0);

    }
    END_TEST                                                     

/* ============================================================
 * Test 11: get indegree with correct status
 * ============================================================ */

START_TEST(test_indegree_status)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    int *c = malloc(sizeof(int));
    *a = 1;
    *b = 2;
    *c = 3;

    graph_insert(g, a);
    graph_insert(g, b);
    graph_insert(g, c);

    graph_connect(g, b, a);
    graph_connect(g, c, a);

    ck_assert_int_eq(graph_indegree(g, a), 2);

    }
    END_TEST

/* ============================================================
 * Test 10: get indegree with 0
 * ============================================================ */

START_TEST(test_indegree_with_zero)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    *a = 1;

    ck_assert_int_eq(graph_indegree(g, a), 0);

    }
    END_TEST

/* ============================================================
 * Test 10: get indegree with NULL
 * ============================================================ */

START_TEST(test_indegree_with_NULL)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    *a = 1;

    ck_assert_int_eq(graph_indegree(NULL, a), 0);

    }
    END_TEST  

/* ============================================================
 * Test 12: graph size with NULL graph
 * ============================================================ */

START_TEST(test_size_null_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;

    graph_insert(g, a);
    graph_insert(g, b);

    graph_connect(g, b, a);

    ck_assert_int_eq(graph_size(NULL), 0);

    }
    END_TEST

/* ============================================================
 * Test 13: graph contains with false on missing payload
 * ============================================================ */

START_TEST(test_contains_false_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;

    graph_insert(g, a);

    ck_assert_int_eq(graph_contains(g, b), 0);
    }
END_TEST

/* ============================================================
 * Test 14: graph contains with NULL graph
 * ============================================================ */

START_TEST(test_contains_null_graph)
{
    // Create two vertices
    int *a = malloc(sizeof(int));
    int *b = malloc(sizeof(int));
    *a = 1;
    *b = 2;

    graph_insert(g, a);
    graph_insert(g, b);

    ck_assert_int_eq(graph_contains(NULL, a), 0);

    }
END_TEST



/* ============================================================
 * Suite Creation Function
 * 
 * This function builds and returns a test suite for the Check framework.
 * A Suite is a collection of test cases that are run together.
 * ============================================================ */

Suite *queries_suite(void)
{
    // Create a new test suite with a descriptive name
    Suite *s = suite_create("GraphQueries");
    
    // Create a test case to group related tests
    TCase *tc = tcase_create("Basics");

    // Attach setup and teardown functions to run before/after each test
    // setup_graph() runs before each test to create a fresh graph
    // teardown_graph() runs after each test to clean up the graph
    tcase_add_checked_fixture(tc, setup_graph, teardown_graph);

    // Add individual test functions to the test case
    tcase_add_test(tc, test_neighbors_small_graph);
    tcase_add_test(tc, test_neighbors_null_graph);
    tcase_add_test(tc, test_neighbors_null_payload);
    tcase_add_test(tc, test_neighbors_null_neighbors_out);
    tcase_add_test(tc, test_neighbors_null_count_out);
    tcase_add_test(tc, test_neighbors_missing_payload);
    tcase_add_test(tc, test_neighbors_no_edges);
    tcase_add_test(tc, test_get_all_payloads_empty);
    tcase_add_test(tc, test_get_all_payloads);
    tcase_add_test(tc, test_null_get_all_payloads_graph);
    tcase_add_test(tc, test_null_get_all_payloads_out);
    tcase_add_test(tc, test_null_get_all_payloads_count_out);
    tcase_add_test(tc, test_outdegree_status);
    tcase_add_test(tc, test_outdegree_with_zero);
    tcase_add_test(tc, test_outdegree_with_NULL);
    tcase_add_test(tc, test_indegree_status);
    tcase_add_test(tc, test_indegree_with_zero);
    tcase_add_test(tc, test_indegree_with_NULL);
    tcase_add_test(tc, test_size_null_graph);
    tcase_add_test(tc, test_contains_false_graph);
    tcase_add_test(tc, test_contains_null_graph);

    // Add the test case to the suite
    suite_add_tcase(s, tc);
    
    // Return the complete suite so main() can run it
    return s;
}