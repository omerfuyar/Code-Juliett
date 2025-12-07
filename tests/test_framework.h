#pragma once

#include <stdio.h>
#include <stdbool.h>

typedef struct TestResult
{
    int total;
    int passed;
    int failed;
} TestResult;

static TestResult g_testResult = {0, 0, 0};

#define TEST_ASSERT(condition, message)                                  \
    do                                                                   \
    {                                                                    \
        g_testResult.total++;                                            \
        if (!(condition))                                                \
        {                                                                \
            g_testResult.failed++;                                       \
            printf("  [FAIL] %s (line %d): %s\n", __func__, __LINE__, message); \
        }                                                                \
        else                                                             \
        {                                                                \
            g_testResult.passed++;                                       \
            printf("  [PASS] %s: %s\n", __func__, message);              \
        }                                                                \
    } while (0)

#define TEST_ASSERT_EQUAL(actual, expected, message)                     \
    TEST_ASSERT((actual) == (expected), message)

#define TEST_ASSERT_FLOAT_EQUAL(actual, expected, epsilon, message)      \
    TEST_ASSERT(((actual) >= ((expected) - (epsilon))) && ((actual) <= ((expected) + (epsilon))), message)

#define RUN_TEST(test_func)                                              \
    do                                                                   \
    {                                                                    \
        printf("\nRunning %s...\n", #test_func);                         \
        test_func();                                                     \
    } while (0)

static void test_print_summary(const char *suiteName)
{
    printf("\n========================================\n");
    printf("Test Suite: %s\n", suiteName);
    printf("========================================\n");
    printf("Total:  %d\n", g_testResult.total);
    printf("Passed: %d\n", g_testResult.passed);
    printf("Failed: %d\n", g_testResult.failed);
    printf("========================================\n");
    
    if (g_testResult.failed > 0)
    {
        printf("RESULT: FAILED\n");
    }
    else
    {
        printf("RESULT: ALL TESTS PASSED\n");
    }
}

static int test_get_result()
{
    return g_testResult.failed > 0 ? 1 : 0;
}
