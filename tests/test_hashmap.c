#include "test_framework.h"
#include "utilities/HashMap.h"
#include <string.h>

void test_hashmap_create_destroy()
{
    HashMap map = HashMap_Create("int", sizeof(int), 10);
    
    TEST_ASSERT(map.data != NULL, "HashMap data should be allocated");
    TEST_ASSERT_EQUAL(map.capacity, 10, "HashMap capacity should be 10");
    TEST_ASSERT_EQUAL(map.count, 0, "HashMap count should start at 0");
    TEST_ASSERT_EQUAL(map.sizeOfItem, sizeof(int), "HashMap sizeOfItem should match int size");
    
    HashMap_Destroy(&map);
    TEST_ASSERT(map.data == NULL, "HashMap data should be NULL after destroy");
}

void test_hashmap_register_and_access()
{
    HashMap map = HashMap_Create("int", sizeof(int), 10);
    
    int value1 = 42;
    int value2 = 100;
    
    HashMap_Register(&map, "first", &value1);
    HashMap_Register(&map, "second", &value2);
    
    int *retrieved1 = (int *)HashMap_Access(&map, "first");
    int *retrieved2 = (int *)HashMap_Access(&map, "second");
    
    TEST_ASSERT(retrieved1 != NULL, "Should retrieve first value");
    TEST_ASSERT_EQUAL(*retrieved1, 42, "First value should be 42");
    
    TEST_ASSERT(retrieved2 != NULL, "Should retrieve second value");
    TEST_ASSERT_EQUAL(*retrieved2, 100, "Second value should be 100");
    
    HashMap_Destroy(&map);
}

void test_hashmap_update_value()
{
    HashMap map = HashMap_Create("int", sizeof(int), 10);
    
    int value1 = 42;
    int value2 = 99;
    
    HashMap_Register(&map, "key", &value1);
    
    int *retrieved = (int *)HashMap_Access(&map, "key");
    TEST_ASSERT_EQUAL(*retrieved, 42, "Initial value should be 42");
    
    HashMap_Register(&map, "key", &value2);
    
    retrieved = (int *)HashMap_Access(&map, "key");
    TEST_ASSERT_EQUAL(*retrieved, 99, "Updated value should be 99");
    
    HashMap_Destroy(&map);
}

void test_hashmap_access_nonexistent()
{
    HashMap map = HashMap_Create("int", sizeof(int), 10);
    
    void *result = HashMap_Access(&map, "nonexistent");
    
    TEST_ASSERT(result == NULL, "Accessing nonexistent key should return NULL");
    
    HashMap_Destroy(&map);
}

void test_hashmap_multiple_types()
{
    HashMap intMap = HashMap_Create("int", sizeof(int), 5);
    HashMap floatMap = HashMap_Create("float", sizeof(float), 5);
    
    int intVal = 123;
    float floatVal = 45.67f;
    
    HashMap_Register(&intMap, "int_key", &intVal);
    HashMap_Register(&floatMap, "float_key", &floatVal);
    
    int *retrievedInt = (int *)HashMap_Access(&intMap, "int_key");
    float *retrievedFloat = (float *)HashMap_Access(&floatMap, "float_key");
    
    TEST_ASSERT_EQUAL(*retrievedInt, 123, "Int value should be 123");
    TEST_ASSERT_FLOAT_EQUAL(*retrievedFloat, 45.67f, 0.001f, "Float value should be 45.67");
    
    HashMap_Destroy(&intMap);
    HashMap_Destroy(&floatMap);
}

typedef struct TestStruct
{
    int id;
    float value;
    char name[32];
} TestStruct;

void test_hashmap_with_struct()
{
    HashMap map = HashMap_Create("TestStruct", sizeof(TestStruct), 5);
    
    TestStruct data = {1, 3.14f, "test"};
    HashMap_Register(&map, "struct_key", &data);
    
    TestStruct *retrieved = (TestStruct *)HashMap_Access(&map, "struct_key");
    
    TEST_ASSERT(retrieved != NULL, "Should retrieve struct");
    TEST_ASSERT_EQUAL(retrieved->id, 1, "Struct id should be 1");
    TEST_ASSERT_FLOAT_EQUAL(retrieved->value, 3.14f, 0.001f, "Struct value should be 3.14");
    TEST_ASSERT(strcmp(retrieved->name, "test") == 0, "Struct name should be 'test'");
    
    HashMap_Destroy(&map);
}

void test_hashmap_capacity_expansion()
{
    HashMap map = HashMap_Create("int", sizeof(int), 2);
    
    for (int i = 0; i < 5; i++)
    {
        char key[16];
        snprintf(key, sizeof(key), "key%d", i);
        HashMap_Register(&map, key, &i);
    }
    
    TEST_ASSERT(map.capacity >= 5, "HashMap should expand capacity when needed");
    
    for (int i = 0; i < 5; i++)
    {
        char key[16];
        snprintf(key, sizeof(key), "key%d", i);
        int *value = (int *)HashMap_Access(&map, key);
        TEST_ASSERT(value != NULL, "All values should be retrievable after expansion");
    }
    
    HashMap_Destroy(&map);
}

void test_hashmap_many_entries()
{
    HashMap map = HashMap_Create("int", sizeof(int), 10);
    
    for (int i = 0; i < 20; i++)
    {
        char key[16];
        snprintf(key, sizeof(key), "item_%d", i);
        HashMap_Register(&map, key, &i);
    }
    
    TEST_ASSERT_EQUAL(map.count, 20, "HashMap should contain 20 entries");
    
    for (int i = 0; i < 20; i++)
    {
        char key[16];
        snprintf(key, sizeof(key), "item_%d", i);
        int *value = (int *)HashMap_Access(&map, key);
        TEST_ASSERT(value != NULL, "All entries should be accessible");
        TEST_ASSERT_EQUAL(*value, i, "Each value should match its index");
    }
    
    HashMap_Destroy(&map);
}

int main(void)
{
    printf("========================================\n");
    printf("Romeo Framework - HashMap Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_hashmap_create_destroy);
    RUN_TEST(test_hashmap_register_and_access);
    RUN_TEST(test_hashmap_update_value);
    RUN_TEST(test_hashmap_access_nonexistent);
    RUN_TEST(test_hashmap_multiple_types);
    RUN_TEST(test_hashmap_with_struct);
    RUN_TEST(test_hashmap_capacity_expansion);
    RUN_TEST(test_hashmap_many_entries);
    
    test_print_summary("HashMap Tests");
    return test_get_result();
}
