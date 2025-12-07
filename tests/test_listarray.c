#include "test_framework.h"
#include "utilities/ListArray.h"
#include <string.h>

TestResult g_testResult = {0, 0, 0};

void test_listarray_create_destroy()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    TEST_ASSERT(list.data != NULL, "ListArray data should be allocated");
    TEST_ASSERT_EQUAL(list.capacity, 5, "ListArray capacity should be 5");
    TEST_ASSERT_EQUAL(list.count, 0, "ListArray count should start at 0");
    TEST_ASSERT_EQUAL(list.sizeOfItem, sizeof(int), "ListArray sizeOfItem should match int size");
    
    ListArray_Destroy(&list);
    TEST_ASSERT(list.data == NULL, "ListArray data should be NULL after destroy");
}

void test_listarray_add_and_get()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    int value1 = 10;
    int value2 = 20;
    int value3 = 30;
    
    ListArray_Add(&list, &value1);
    ListArray_Add(&list, &value2);
    ListArray_Add(&list, &value3);
    
    TEST_ASSERT_EQUAL(list.count, 3, "ListArray should have 3 items");
    
    int *retrieved1 = (int *)ListArray_Get(&list, 0);
    int *retrieved2 = (int *)ListArray_Get(&list, 1);
    int *retrieved3 = (int *)ListArray_Get(&list, 2);
    
    TEST_ASSERT_EQUAL(*retrieved1, 10, "First item should be 10");
    TEST_ASSERT_EQUAL(*retrieved2, 20, "Second item should be 20");
    TEST_ASSERT_EQUAL(*retrieved3, 30, "Third item should be 30");
    
    ListArray_Destroy(&list);
}

void test_listarray_set()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    int values[3] = {10, 20, 30};
    for (int i = 0; i < 3; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    int newValue = 99;
    ListArray_Set(&list, 1, &newValue);
    
    int *retrieved = (int *)ListArray_Get(&list, 1);
    TEST_ASSERT_EQUAL(*retrieved, 99, "Second item should be updated to 99");
    
    ListArray_Destroy(&list);
}

void test_listarray_add_range()
{
    ListArray list = ListArray_Create("int", sizeof(int), 10);
    
    int values[5] = {1, 2, 3, 4, 5};
    ListArray_AddRange(&list, values, 5);
    
    TEST_ASSERT_EQUAL(list.count, 5, "ListArray should have 5 items");
    
    for (int i = 0; i < 5; i++)
    {
        int *val = (int *)ListArray_Get(&list, i);
        TEST_ASSERT_EQUAL(*val, i + 1, "Values should match the added range");
    }
    
    ListArray_Destroy(&list);
}

void test_listarray_remove_at_index()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    int values[5] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    ListArray_RemoveAtIndex(&list, 2);
    
    TEST_ASSERT_EQUAL(list.count, 4, "ListArray should have 4 items after removal");
    
    int *val3 = (int *)ListArray_Get(&list, 2);
    TEST_ASSERT_EQUAL(*val3, 40, "Third position should now contain 40");
    
    ListArray_Destroy(&list);
}

void test_listarray_remove_range()
{
    ListArray list = ListArray_Create("int", sizeof(int), 10);
    
    int values[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int i = 0; i < 10; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    ListArray_RemoveRange(&list, 3, 4);
    
    TEST_ASSERT_EQUAL(list.count, 6, "ListArray should have 6 items after removing 4");
    
    int *val3 = (int *)ListArray_Get(&list, 3);
    TEST_ASSERT_EQUAL(*val3, 7, "Fourth position should now contain 7");
    
    ListArray_Destroy(&list);
}

void test_listarray_pop()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    int values[3] = {10, 20, 30};
    for (int i = 0; i < 3; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    int *popped = (int *)ListArray_Pop(&list);
    
    TEST_ASSERT(popped != NULL, "Pop should return a value");
    TEST_ASSERT_EQUAL(*popped, 30, "Popped value should be 30");
    TEST_ASSERT_EQUAL(list.count, 2, "ListArray should have 2 items after pop");
    
    ListArray_Destroy(&list);
}

void test_listarray_clear()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    int values[3] = {10, 20, 30};
    for (int i = 0; i < 3; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    ListArray_Clear(&list);
    
    TEST_ASSERT_EQUAL(list.count, 0, "ListArray count should be 0 after clear");
    TEST_ASSERT(list.data != NULL, "ListArray data should not be NULL after clear");
    
    ListArray_Destroy(&list);
}

void test_listarray_index_of()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    int values[5] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    int searchValue = 30;
    long long index = ListArray_IndexOf(&list, &searchValue);
    
    TEST_ASSERT_EQUAL(index, 2, "Index of 30 should be 2");
    
    searchValue = 99;
    index = ListArray_IndexOf(&list, &searchValue);
    
    TEST_ASSERT_EQUAL(index, -1, "Index of nonexistent value should be -1");
    
    ListArray_Destroy(&list);
}

void test_listarray_remove_item()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    int values[5] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    int removeValue = 30;
    ListArray_RemoveItem(&list, &removeValue);
    
    TEST_ASSERT_EQUAL(list.count, 4, "ListArray should have 4 items after removing item");
    
    long long index = ListArray_IndexOf(&list, &removeValue);
    TEST_ASSERT_EQUAL(index, -1, "Removed item should not be found");
    
    ListArray_Destroy(&list);
}

void test_listarray_copy()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    int values[3] = {10, 20, 30};
    for (int i = 0; i < 3; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    ListArray copy = ListArray_Copy(&list);
    
    TEST_ASSERT(copy.data != list.data, "Copy should have different data pointer");
    TEST_ASSERT_EQUAL(copy.count, list.count, "Copy should have same count");
    TEST_ASSERT_EQUAL(copy.capacity, list.capacity, "Copy should have same capacity");
    
    for (RJGlobal_Size i = 0; i < list.count; i++)
    {
        int *original = (int *)ListArray_Get(&list, i);
        int *copied = (int *)ListArray_Get(&copy, i);
        TEST_ASSERT_EQUAL(*copied, *original, "Copied values should match original");
    }
    
    ListArray_Destroy(&list);
    ListArray_Destroy(&copy);
}

void test_listarray_resize()
{
    ListArray list = ListArray_Create("int", sizeof(int), 5);
    
    int values[3] = {10, 20, 30};
    for (int i = 0; i < 3; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    ListArray_Resize(&list, 10);
    
    TEST_ASSERT_EQUAL(list.capacity, 10, "Capacity should be resized to 10");
    TEST_ASSERT_EQUAL(list.count, 3, "Count should remain 3");
    
    for (int i = 0; i < 3; i++)
    {
        int *val = (int *)ListArray_Get(&list, i);
        TEST_ASSERT_EQUAL(*val, values[i], "Values should be preserved after resize");
    }
    
    ListArray_Destroy(&list);
}

void test_listarray_add_to_index()
{
    ListArray list = ListArray_Create("int", sizeof(int), 10);
    
    int values[3] = {10, 20, 30};
    for (int i = 0; i < 3; i++)
    {
        ListArray_Add(&list, &values[i]);
    }
    
    int newValue = 15;
    ListArray_AddToIndex(&list, 1, &newValue);
    
    TEST_ASSERT_EQUAL(list.count, 4, "ListArray should have 4 items");
    
    int *val1 = (int *)ListArray_Get(&list, 1);
    TEST_ASSERT_EQUAL(*val1, 15, "Value at index 1 should be 15");
    
    int *val2 = (int *)ListArray_Get(&list, 2);
    TEST_ASSERT_EQUAL(*val2, 20, "Value at index 2 should be 20");
    
    ListArray_Destroy(&list);
}

int main(void)
{
    printf("========================================\n");
    printf("Romeo Framework - ListArray Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_listarray_create_destroy);
    RUN_TEST(test_listarray_add_and_get);
    RUN_TEST(test_listarray_set);
    RUN_TEST(test_listarray_add_range);
    RUN_TEST(test_listarray_remove_at_index);
    RUN_TEST(test_listarray_remove_range);
    RUN_TEST(test_listarray_pop);
    RUN_TEST(test_listarray_clear);
    RUN_TEST(test_listarray_index_of);
    RUN_TEST(test_listarray_remove_item);
    RUN_TEST(test_listarray_copy);
    RUN_TEST(test_listarray_resize);
    RUN_TEST(test_listarray_add_to_index);
    
    test_print_summary("ListArray Tests");
    return test_get_result();
}
