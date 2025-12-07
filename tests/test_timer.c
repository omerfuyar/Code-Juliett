#include "test_framework.h"
#include "utilities/Timer.h"
#include <time.h>

#if defined(_WIN32)
#include <windows.h>
#define sleep_ms(ms) Sleep(ms)
#else
#include <unistd.h>
#define sleep_ms(ms) usleep((ms) * 1000)
#endif

void test_timer_create_destroy()
{
    Timer timer = Timer_Create("Test Timer");
    
    TEST_ASSERT(timer.title != NULL, "Timer title should not be NULL");
    TEST_ASSERT(!timer.isRunning, "Timer should not be running initially");
    
    Timer_Destroy(&timer);
    TEST_ASSERT(timer.title == NULL, "Timer title should be NULL after destroy");
}

void test_timer_start_stop()
{
    Timer timer = Timer_Create("Start-Stop Timer");
    
    Timer_Start(&timer);
    TEST_ASSERT(timer.isRunning, "Timer should be running after start");
    
    sleep_ms(10);
    
    Timer_Stop(&timer);
    TEST_ASSERT(!timer.isRunning, "Timer should not be running after stop");
    
    Timer_Destroy(&timer);
}

void test_timer_elapsed_time()
{
    Timer timer = Timer_Create("Elapsed Timer");
    
    Timer_Start(&timer);
    sleep_ms(100);
    Timer_Stop(&timer);
    
    TimePoint elapsed = Timer_GetElapsedTime(&timer);
    
    TEST_ASSERT(elapsed.seconds >= 0, "Elapsed seconds should be non-negative");
    TEST_ASSERT(elapsed.nanoseconds >= 0, "Elapsed nanoseconds should be non-negative");
    
    Timer_Destroy(&timer);
}

void test_timer_elapsed_milliseconds()
{
    Timer timer = Timer_Create("Milliseconds Timer");
    
    Timer_Start(&timer);
    sleep_ms(50);
    Timer_Stop(&timer);
    
    float elapsed = Timer_GetElapsedMilliseconds(&timer);
    
    TEST_ASSERT(elapsed >= 40.0f, "Elapsed time should be at least 40ms");
    TEST_ASSERT(elapsed < 200.0f, "Elapsed time should be less than 200ms");
    
    Timer_Destroy(&timer);
}

void test_timer_elapsed_nanoseconds()
{
    Timer timer = Timer_Create("Nanoseconds Timer");
    
    Timer_Start(&timer);
    sleep_ms(10);
    Timer_Stop(&timer);
    
    time_t elapsed = Timer_GetElapsedNanoseconds(&timer);
    
    TEST_ASSERT(elapsed > 0, "Elapsed nanoseconds should be positive");
    
    Timer_Destroy(&timer);
}

void test_timer_reset()
{
    Timer timer = Timer_Create("Reset Timer");
    
    Timer_Start(&timer);
    sleep_ms(50);
    Timer_Stop(&timer);
    
    float firstElapsed = Timer_GetElapsedMilliseconds(&timer);
    
    Timer_Reset(&timer);
    Timer_Start(&timer);
    sleep_ms(10);
    Timer_Stop(&timer);
    
    float secondElapsed = Timer_GetElapsedMilliseconds(&timer);
    
    TEST_ASSERT(secondElapsed < firstElapsed, "Second elapsed time should be less after reset");
    TEST_ASSERT(secondElapsed < 50.0f, "Second elapsed should be small after reset");
    
    Timer_Destroy(&timer);
}

void test_timepoint_update()
{
    TimePoint tp1;
    TimePoint_Update(&tp1);
    
    sleep_ms(10);
    
    TimePoint tp2;
    TimePoint_Update(&tp2);
    
    TEST_ASSERT(tp2.seconds >= tp1.seconds, "Second timepoint should be later");
    TEST_ASSERT(tp1.seconds >= 0, "Timepoint seconds should be valid");
}

void test_timepoint_to_milliseconds()
{
    TimePoint tp;
    tp.seconds = 1;
    tp.nanoseconds = 500000000;
    
    float ms = TimePoint_ToMilliseconds(&tp);
    
    TEST_ASSERT_FLOAT_EQUAL(ms, 1500.0f, 1.0f, "1.5 seconds should be 1500 milliseconds");
}

void test_timer_multiple_start_stop()
{
    Timer timer = Timer_Create("Multiple Start-Stop Timer");
    
    Timer_Start(&timer);
    sleep_ms(20);
    Timer_Stop(&timer);
    
    float firstElapsed = Timer_GetElapsedMilliseconds(&timer);
    
    Timer_Start(&timer);
    sleep_ms(20);
    Timer_Stop(&timer);
    
    float secondElapsed = Timer_GetElapsedMilliseconds(&timer);
    
    TEST_ASSERT(firstElapsed > 0.0f, "First elapsed should be positive");
    TEST_ASSERT(secondElapsed > 0.0f, "Second elapsed should be positive");
    
    Timer_Destroy(&timer);
}

void test_timer_precision()
{
    Timer timer = Timer_Create("Precision Timer");
    
    Timer_Start(&timer);
    sleep_ms(100);
    Timer_Stop(&timer);
    
    float elapsed = Timer_GetElapsedMilliseconds(&timer);
    
    TEST_ASSERT(elapsed >= 80.0f, "Timer should measure at least 80ms");
    TEST_ASSERT(elapsed <= 200.0f, "Timer should not measure more than 200ms");
    
    Timer_Destroy(&timer);
}

int main(void)
{
    printf("========================================\n");
    printf("Romeo Framework - Timer Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_timer_create_destroy);
    RUN_TEST(test_timer_start_stop);
    RUN_TEST(test_timer_elapsed_time);
    RUN_TEST(test_timer_elapsed_milliseconds);
    RUN_TEST(test_timer_elapsed_nanoseconds);
    RUN_TEST(test_timer_reset);
    RUN_TEST(test_timepoint_update);
    RUN_TEST(test_timepoint_to_milliseconds);
    RUN_TEST(test_timer_multiple_start_stop);
    RUN_TEST(test_timer_precision);
    
    test_print_summary("Timer Tests");
    return test_get_result();
}
