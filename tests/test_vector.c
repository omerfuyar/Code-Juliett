#include "test_framework.h"
#include "utilities/Vector.h"
#include <math.h>

TestResult g_testResult = {0, 0, 0};

#define EPSILON 0.0001f

void test_vector2_creation()
{
    Vector2 v = Vector2_New(3.0f, 4.0f);
    TEST_ASSERT_FLOAT_EQUAL(v.x, 3.0f, EPSILON, "Vector2 x component should be 3.0");
    TEST_ASSERT_FLOAT_EQUAL(v.y, 4.0f, EPSILON, "Vector2 y component should be 4.0");
}

void test_vector2_zero()
{
    Vector2 v = Vector2_Zero;
    TEST_ASSERT_FLOAT_EQUAL(v.x, 0.0f, EPSILON, "Vector2_Zero x component should be 0.0");
    TEST_ASSERT_FLOAT_EQUAL(v.y, 0.0f, EPSILON, "Vector2_Zero y component should be 0.0");
}

void test_vector2_one()
{
    Vector2 v = Vector2_One;
    TEST_ASSERT_FLOAT_EQUAL(v.x, 1.0f, EPSILON, "Vector2_One x component should be 1.0");
    TEST_ASSERT_FLOAT_EQUAL(v.y, 1.0f, EPSILON, "Vector2_One y component should be 1.0");
}

void test_vector2_add()
{
    Vector2 v1 = Vector2_New(1.0f, 2.0f);
    Vector2 v2 = Vector2_New(3.0f, 4.0f);
    Vector2 result = Vector2_Add(v1, v2);
    TEST_ASSERT_FLOAT_EQUAL(result.x, 4.0f, EPSILON, "Vector2_Add x should be 4.0");
    TEST_ASSERT_FLOAT_EQUAL(result.y, 6.0f, EPSILON, "Vector2_Add y should be 6.0");
}

void test_vector2_scale()
{
    Vector2 v = Vector2_New(2.0f, 3.0f);
    Vector2 result = Vector2_Scale(v, 2.0f);
    TEST_ASSERT_FLOAT_EQUAL(result.x, 4.0f, EPSILON, "Vector2_Scale x should be 4.0");
    TEST_ASSERT_FLOAT_EQUAL(result.y, 6.0f, EPSILON, "Vector2_Scale y should be 6.0");
}

void test_vector2_magnitude()
{
    Vector2 v = Vector2_New(3.0f, 4.0f);
    float mag = Vector2_Magnitude(v);
    TEST_ASSERT_FLOAT_EQUAL(mag, 5.0f, EPSILON, "Vector2_Magnitude should be 5.0 for (3,4)");
}

void test_vector2_normalized()
{
    Vector2 v = Vector2_New(3.0f, 4.0f);
    Vector2 result = Vector2_Normalized(v);
    float mag = Vector2_Magnitude(result);
    TEST_ASSERT_FLOAT_EQUAL(mag, 1.0f, EPSILON, "Normalized vector magnitude should be 1.0");
    TEST_ASSERT_FLOAT_EQUAL(result.x, 0.6f, EPSILON, "Normalized x should be 0.6");
    TEST_ASSERT_FLOAT_EQUAL(result.y, 0.8f, EPSILON, "Normalized y should be 0.8");
}

void test_vector2_dot()
{
    Vector2 v1 = Vector2_New(1.0f, 2.0f);
    Vector2 v2 = Vector2_New(3.0f, 4.0f);
    float dot = Vector2_Dot(v1, v2);
    TEST_ASSERT_FLOAT_EQUAL(dot, 11.0f, EPSILON, "Dot product should be 11.0 (1*3 + 2*4)");
}

void test_vector2_lerp()
{
    Vector2 start = Vector2_New(0.0f, 0.0f);
    Vector2 end = Vector2_New(10.0f, 10.0f);
    Vector2 mid = Vector2_Lerp(start, end, 0.5f);
    TEST_ASSERT_FLOAT_EQUAL(mid.x, 5.0f, EPSILON, "Lerp at 0.5 should give x=5.0");
    TEST_ASSERT_FLOAT_EQUAL(mid.y, 5.0f, EPSILON, "Lerp at 0.5 should give y=5.0");
}

void test_vector3_creation()
{
    Vector3 v = Vector3_New(1.0f, 2.0f, 3.0f);
    TEST_ASSERT_FLOAT_EQUAL(v.x, 1.0f, EPSILON, "Vector3 x component should be 1.0");
    TEST_ASSERT_FLOAT_EQUAL(v.y, 2.0f, EPSILON, "Vector3 y component should be 2.0");
    TEST_ASSERT_FLOAT_EQUAL(v.z, 3.0f, EPSILON, "Vector3 z component should be 3.0");
}

void test_vector3_directional()
{
    Vector3 up = Vector3_Up;
    TEST_ASSERT_FLOAT_EQUAL(up.y, 1.0f, EPSILON, "Vector3_Up y should be 1.0");
    
    Vector3 right = Vector3_Right;
    TEST_ASSERT_FLOAT_EQUAL(right.x, 1.0f, EPSILON, "Vector3_Right x should be 1.0");
    
    Vector3 forward = Vector3_Forward;
    TEST_ASSERT_FLOAT_EQUAL(forward.z, 1.0f, EPSILON, "Vector3_Forward z should be 1.0");
}

void test_vector3_add()
{
    Vector3 v1 = Vector3_New(1.0f, 2.0f, 3.0f);
    Vector3 v2 = Vector3_New(4.0f, 5.0f, 6.0f);
    Vector3 result = Vector3_Add(v1, v2);
    TEST_ASSERT_FLOAT_EQUAL(result.x, 5.0f, EPSILON, "Vector3_Add x should be 5.0");
    TEST_ASSERT_FLOAT_EQUAL(result.y, 7.0f, EPSILON, "Vector3_Add y should be 7.0");
    TEST_ASSERT_FLOAT_EQUAL(result.z, 9.0f, EPSILON, "Vector3_Add z should be 9.0");
}

void test_vector3_scale()
{
    Vector3 v = Vector3_New(1.0f, 2.0f, 3.0f);
    Vector3 result = Vector3_Scale(v, 2.0f);
    TEST_ASSERT_FLOAT_EQUAL(result.x, 2.0f, EPSILON, "Vector3_Scale x should be 2.0");
    TEST_ASSERT_FLOAT_EQUAL(result.y, 4.0f, EPSILON, "Vector3_Scale y should be 4.0");
    TEST_ASSERT_FLOAT_EQUAL(result.z, 6.0f, EPSILON, "Vector3_Scale z should be 6.0");
}

void test_vector3_magnitude()
{
    Vector3 v = Vector3_New(2.0f, 3.0f, 6.0f);
    float mag = Vector3_Magnitude(v);
    TEST_ASSERT_FLOAT_EQUAL(mag, 7.0f, EPSILON, "Vector3_Magnitude should be 7.0");
}

void test_vector3_normalized()
{
    Vector3 v = Vector3_New(2.0f, 3.0f, 6.0f);
    Vector3 result = Vector3_Normalized(v);
    float mag = Vector3_Magnitude(result);
    TEST_ASSERT_FLOAT_EQUAL(mag, 1.0f, EPSILON, "Normalized vector3 magnitude should be 1.0");
}

void test_vector3_dot()
{
    Vector3 v1 = Vector3_New(1.0f, 2.0f, 3.0f);
    Vector3 v2 = Vector3_New(4.0f, 5.0f, 6.0f);
    float dot = Vector3_Dot(v1, v2);
    TEST_ASSERT_FLOAT_EQUAL(dot, 32.0f, EPSILON, "Dot product should be 32.0 (1*4 + 2*5 + 3*6)");
}

void test_vector3_cross()
{
    Vector3 v1 = Vector3_New(1.0f, 0.0f, 0.0f);
    Vector3 v2 = Vector3_New(0.0f, 1.0f, 0.0f);
    Vector3 result = Vector3_Cross(v1, v2);
    TEST_ASSERT_FLOAT_EQUAL(result.x, 0.0f, EPSILON, "Cross product x should be 0.0");
    TEST_ASSERT_FLOAT_EQUAL(result.y, 0.0f, EPSILON, "Cross product y should be 0.0");
    TEST_ASSERT_FLOAT_EQUAL(result.z, 1.0f, EPSILON, "Cross product z should be 1.0");
}

void test_vector3_lerp()
{
    Vector3 start = Vector3_Zero;
    Vector3 end = Vector3_New(10.0f, 20.0f, 30.0f);
    Vector3 result = Vector3_Lerp(start, end, 0.5f);
    TEST_ASSERT_FLOAT_EQUAL(result.x, 5.0f, EPSILON, "Lerp x at 0.5 should be 5.0");
    TEST_ASSERT_FLOAT_EQUAL(result.y, 10.0f, EPSILON, "Lerp y at 0.5 should be 10.0");
    TEST_ASSERT_FLOAT_EQUAL(result.z, 15.0f, EPSILON, "Lerp z at 0.5 should be 15.0");
}

void test_vector4_creation()
{
    Vector4 v = Vector4_New(1.0f, 2.0f, 3.0f, 4.0f);
    TEST_ASSERT_FLOAT_EQUAL(v.x, 1.0f, EPSILON, "Vector4 x component should be 1.0");
    TEST_ASSERT_FLOAT_EQUAL(v.y, 2.0f, EPSILON, "Vector4 y component should be 2.0");
    TEST_ASSERT_FLOAT_EQUAL(v.z, 3.0f, EPSILON, "Vector4 z component should be 3.0");
    TEST_ASSERT_FLOAT_EQUAL(v.w, 4.0f, EPSILON, "Vector4 w component should be 4.0");
}

void test_color_creation()
{
    Color c = Color_White;
    TEST_ASSERT_FLOAT_EQUAL(c.x, 1.0f, EPSILON, "White color r should be 1.0");
    TEST_ASSERT_FLOAT_EQUAL(c.y, 1.0f, EPSILON, "White color g should be 1.0");
    TEST_ASSERT_FLOAT_EQUAL(c.z, 1.0f, EPSILON, "White color b should be 1.0");
    TEST_ASSERT_FLOAT_EQUAL(c.w, 1.0f, EPSILON, "White color a should be 1.0");
}

void test_vector4_add()
{
    Vector4 v1 = Vector4_New(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4 v2 = Vector4_New(5.0f, 6.0f, 7.0f, 8.0f);
    Vector4 result = Vector4_Add(v1, v2);
    TEST_ASSERT_FLOAT_EQUAL(result.x, 6.0f, EPSILON, "Vector4_Add x should be 6.0");
    TEST_ASSERT_FLOAT_EQUAL(result.y, 8.0f, EPSILON, "Vector4_Add y should be 8.0");
    TEST_ASSERT_FLOAT_EQUAL(result.z, 10.0f, EPSILON, "Vector4_Add z should be 10.0");
    TEST_ASSERT_FLOAT_EQUAL(result.w, 12.0f, EPSILON, "Vector4_Add w should be 12.0");
}

void test_vector4_scale()
{
    Vector4 v = Vector4_New(1.0f, 2.0f, 3.0f, 4.0f);
    Vector4 result = Vector4_Scale(v, 2.0f);
    TEST_ASSERT_FLOAT_EQUAL(result.x, 2.0f, EPSILON, "Vector4_Scale x should be 2.0");
    TEST_ASSERT_FLOAT_EQUAL(result.y, 4.0f, EPSILON, "Vector4_Scale y should be 4.0");
    TEST_ASSERT_FLOAT_EQUAL(result.z, 6.0f, EPSILON, "Vector4_Scale z should be 6.0");
    TEST_ASSERT_FLOAT_EQUAL(result.w, 8.0f, EPSILON, "Vector4_Scale w should be 8.0");
}

void test_vector2int_operations()
{
    Vector2Int v1 = Vector2Int_New(3, 4);
    Vector2Int v2 = Vector2Int_New(1, 2);
    Vector2Int result = Vector2Int_Add(v1, v2);
    
    TEST_ASSERT_EQUAL(result.x, 4, "Vector2Int_Add x should be 4");
    TEST_ASSERT_EQUAL(result.y, 6, "Vector2Int_Add y should be 6");
    
    float mag = Vector2Int_Magnitude(v1);
    TEST_ASSERT_FLOAT_EQUAL(mag, 5.0f, EPSILON, "Vector2Int_Magnitude should be 5.0 for (3,4)");
}

void test_vector3int_operations()
{
    Vector3Int v1 = Vector3Int_New(1, 2, 3);
    Vector3Int v2 = Vector3Int_New(4, 5, 6);
    Vector3Int result = Vector3Int_Add(v1, v2);
    
    TEST_ASSERT_EQUAL(result.x, 5, "Vector3Int_Add x should be 5");
    TEST_ASSERT_EQUAL(result.y, 7, "Vector3Int_Add y should be 7");
    TEST_ASSERT_EQUAL(result.z, 9, "Vector3Int_Add z should be 9");
}

int main(void)
{
    printf("========================================\n");
    printf("Romeo Framework - Vector Tests\n");
    printf("========================================\n");
    
    RUN_TEST(test_vector2_creation);
    RUN_TEST(test_vector2_zero);
    RUN_TEST(test_vector2_one);
    RUN_TEST(test_vector2_add);
    RUN_TEST(test_vector2_scale);
    RUN_TEST(test_vector2_magnitude);
    RUN_TEST(test_vector2_normalized);
    RUN_TEST(test_vector2_dot);
    RUN_TEST(test_vector2_lerp);
    
    RUN_TEST(test_vector3_creation);
    RUN_TEST(test_vector3_directional);
    RUN_TEST(test_vector3_add);
    RUN_TEST(test_vector3_scale);
    RUN_TEST(test_vector3_magnitude);
    RUN_TEST(test_vector3_normalized);
    RUN_TEST(test_vector3_dot);
    RUN_TEST(test_vector3_cross);
    RUN_TEST(test_vector3_lerp);
    
    RUN_TEST(test_vector4_creation);
    RUN_TEST(test_color_creation);
    RUN_TEST(test_vector4_add);
    RUN_TEST(test_vector4_scale);
    
    RUN_TEST(test_vector2int_operations);
    RUN_TEST(test_vector3int_operations);
    
    test_print_summary("Vector Tests");
    return test_get_result();
}
