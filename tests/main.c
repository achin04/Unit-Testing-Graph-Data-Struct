#include <check.h>


// Forward declaration of the function that creates the test suite
// This function must be defined in the respective test suite file
Suite *starter_suite(void);
Suite *insertion_suite(void);
Suite *edge_suite(void);
Suite *queries_suite(void);
Suite *algorithms_suite(void);
Suite *remove_suite(void);


// Entry point for running all test suites
int main(void) {
    // Array of test suites to run. Add more suites to the array as you write them.
    Suite *suites[] = { starter_suite(), insertion_suite(), edge_suite(), queries_suite(), algorithms_suite(), remove_suite(), NULL };

    // The test runner must be created with an initial suite (cannot start empty),
    // so we use the first suite in the array to initialize it.
    SRunner *runner = srunner_create(suites[0]);

    /* If there are more suites, add them to the runner one by one.
     This two-step process is required because srunner_create() needs a non-NULL suite to start,
     and additional suites can only be added after the runner is created.*/
    for (int i = 1; suites[i] != NULL; ++i) {
        srunner_add_suite(runner, suites[i]);
    }

    // Run all tests in normal mode
    srunner_run_all(runner, CK_NORMAL);

    // Get the number of failed tests
    int failed = srunner_ntests_failed(runner);

    // Clean up the test runner
    srunner_free(runner);

    // Return 1 if any tests failed, 0 otherwise
    if (failed) {
        return 1;
    }
    return 0;
}
