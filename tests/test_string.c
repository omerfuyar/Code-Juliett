#include "test_framework.h"
#include "utilities/String.h"
#include <string.h>

void test_string_create_copy()
{
    const char *testStr = "Hello, World!";
    String str = String_CreateCopySafe(testStr, (RJGlobal_Size)strlen(testStr));
    
    TEST_ASSERT(str.characters != NULL, "String characters should not be NULL");
    TEST_ASSERT_EQUAL(str.length, 13, "String length should be 13");
    TEST_ASSERT(strcmp(str.characters, "Hello, World!") == 0, "String content should match");
    
    String_Destroy(&str);
}

void test_string_view_from_literal()
{
    StringView view = scl("Test String");
    
    TEST_ASSERT(view.characters != NULL, "StringView characters should not be NULL");
    TEST_ASSERT_EQUAL(view.length, 11, "StringView length should be 11");
}

void test_string_compare()
{
    StringView str1 = scl("apple");
    StringView str2 = scl("apple");
    StringView str3 = scl("banana");
    
    TEST_ASSERT_EQUAL(String_Compare(str1, str2), 0, "Identical strings should compare equal");
    TEST_ASSERT(String_Compare(str1, str3) < 0, "apple should be less than banana");
    TEST_ASSERT(String_Compare(str3, str1) > 0, "banana should be greater than apple");
}

void test_string_are_same()
{
    StringView str1 = scl("hello");
    StringView str2 = scl("hello");
    StringView str3 = scl("world");
    
    TEST_ASSERT(String_AreSame(str1, str2), "Identical strings should be same");
    TEST_ASSERT(!String_AreSame(str1, str3), "Different strings should not be same");
}

void test_string_concat_end()
{
    String str = String_CreateCopySafe("Hello", 5);
    StringView append = scl(", World!");
    
    String_ConcatEnd(&str, append);
    
    TEST_ASSERT_EQUAL(str.length, 13, "Concatenated string length should be 13");
    TEST_ASSERT(strcmp(str.characters, "Hello, World!") == 0, "Concatenated string should match");
    
    String_Destroy(&str);
}

void test_string_concat_begin()
{
    String str = String_CreateCopySafe("World!", 6);
    StringView prepend = scl("Hello, ");
    
    String_ConcatBegin(&str, prepend);
    
    TEST_ASSERT_EQUAL(str.length, 13, "Concatenated string length should be 13");
    TEST_ASSERT(strcmp(str.characters, "Hello, World!") == 0, "Prepended string should match");
    
    String_Destroy(&str);
}

void test_string_change()
{
    String str = String_CreateCopySafe("Initial", 7);
    StringView newContent = scl("Changed");
    
    String_Change(&str, newContent);
    
    TEST_ASSERT_EQUAL(str.length, 7, "Changed string length should be 7");
    TEST_ASSERT(strcmp(str.characters, "Changed") == 0, "Changed string should match");
    
    String_Destroy(&str);
}

void test_string_get_char()
{
    StringView str = scl("Hello");
    
    TEST_ASSERT_EQUAL(String_GetChar(str, 0), 'H', "First character should be 'H'");
    TEST_ASSERT_EQUAL(String_GetChar(str, 4), 'o', "Last character should be 'o'");
}

void test_string_to_float()
{
    StringView str1 = scl("123.456");
    StringView str2 = scl("-78.9");
    StringView str3 = scl("not a number");
    
    float f1 = String_ToFloat(str1);
    float f2 = String_ToFloat(str2);
    float f3 = String_ToFloat(str3);
    
    TEST_ASSERT_FLOAT_EQUAL(f1, 123.456f, 0.001f, "String_ToFloat should parse positive float");
    TEST_ASSERT_FLOAT_EQUAL(f2, -78.9f, 0.001f, "String_ToFloat should parse negative float");
    TEST_ASSERT_FLOAT_EQUAL(f3, 0.0f, 0.001f, "String_ToFloat should return 0.0 for invalid input");
}

void test_string_to_int()
{
    StringView str1 = scl("42");
    StringView str2 = scl("-100");
    StringView str3 = scl("invalid");
    
    int i1 = String_ToInt(str1);
    int i2 = String_ToInt(str2);
    int i3 = String_ToInt(str3);
    
    TEST_ASSERT_EQUAL(i1, 42, "String_ToInt should parse positive integer");
    TEST_ASSERT_EQUAL(i2, -100, "String_ToInt should parse negative integer");
    TEST_ASSERT_EQUAL(i3, 0, "String_ToInt should return 0 for invalid input");
}

void test_string_tokenize()
{
    StringView str = scl("apple,banana,cherry");
    StringView delimiter = scl(",");
    StringView tokens[5];
    RJGlobal_Size tokenCount = 0;
    
    String_Tokenize(str, delimiter, &tokenCount, tokens, 5);
    
    TEST_ASSERT_EQUAL(tokenCount, 3, "Should have 3 tokens");
    TEST_ASSERT(String_AreSame(tokens[0], scl("apple")), "First token should be 'apple'");
    TEST_ASSERT(String_AreSame(tokens[1], scl("banana")), "Second token should be 'banana'");
    TEST_ASSERT(String_AreSame(tokens[2], scl("cherry")), "Third token should be 'cherry'");
}

void test_string_copy_macro()
{
    String original = String_CreateCopySafe("Original", 8);
    String copy = scc(original);
    
    TEST_ASSERT(copy.characters != original.characters, "Copy should have different memory");
    TEST_ASSERT_EQUAL(copy.length, original.length, "Copy should have same length");
    TEST_ASSERT(strcmp(copy.characters, original.characters) == 0, "Copy should have same content");
    
    String_Destroy(&original);
    String_Destroy(&copy);
}

void test_string_view_creation()
{
    String str = String_CreateCopySafe("Test", 4);
    StringView view = scv(str);
    
    TEST_ASSERT(view.characters == str.characters, "View should point to same memory");
    TEST_ASSERT_EQUAL(view.length, str.length, "View should have same length");
    
    String_Destroy(&str);
}

int main(void)
{
    printf("========================================\n");
    printf("Romeo Framework - String Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_string_create_copy);
    RUN_TEST(test_string_view_from_literal);
    RUN_TEST(test_string_compare);
    RUN_TEST(test_string_are_same);
    RUN_TEST(test_string_concat_end);
    RUN_TEST(test_string_concat_begin);
    RUN_TEST(test_string_change);
    RUN_TEST(test_string_get_char);
    RUN_TEST(test_string_to_float);
    RUN_TEST(test_string_to_int);
    RUN_TEST(test_string_tokenize);
    RUN_TEST(test_string_copy_macro);
    RUN_TEST(test_string_view_creation);
    
    test_print_summary("String Tests");
    return test_get_result();
}
