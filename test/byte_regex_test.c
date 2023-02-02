#include "unity.h"
#include "byte_regex.c"

#include <errno.h>

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void test_compile_pattern_0()
{
    unsigned int pattern_length = 32;
    uint8_t compiled_pattern[32];
    int return_code = byte_regex_compile_pattern("0F{18}", compiled_pattern, &pattern_length);

    UNITY_TEST_ASSERT_EQUAL_INT(0, return_code, __LINE__, "Unexpected return code");
    UNITY_TEST_ASSERT_EQUAL_STRING("\xF2\x0F\x12\x0F", compiled_pattern, __LINE__, "Unexpected compiled pattern");
    UNITY_TEST_ASSERT_EQUAL_UINT(4, pattern_length, __LINE__, "Unexpected pattern length");
}

void test_validate_pattern()
{
    UNITY_TEST_ASSERT_EQUAL_INT(-EINVAL, byte_regex_validate_pattern(""), __LINE__, "Empty pattern should return -EINVAL");
    UNITY_TEST_ASSERT_EQUAL_INT(-EINVAL, byte_regex_validate_pattern(NULL), __LINE__, "NULL pattern should return -EINVAL");
    UNITY_TEST_ASSERT_EQUAL_INT(0, byte_regex_validate_pattern("\xF2\x0F\x12\x0F"), __LINE__, "Pattern should be OK (pattern = 0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F0F)");
    UNITY_TEST_ASSERT_EQUAL_INT(0, byte_regex_validate_pattern("\x11\xF1\x31"), __LINE__, "Pattern should be OK (pattern = F131)");
    UNITY_TEST_ASSERT_EQUAL_INT(0, byte_regex_validate_pattern("\x11\xF1\x21"), __LINE__, "Pattern should be OK (pattern = F121)");
    UNITY_TEST_ASSERT_EQUAL_INT(0, byte_regex_validate_pattern("\x11\xF1\x41"), __LINE__, "Pattern should be OK (pattern = F141)");
    UNITY_TEST_ASSERT_EQUAL_INT(0, byte_regex_validate_pattern("\x11\xF1\x51"), __LINE__, "Pattern should be OK (pattern = F151)");
    UNITY_TEST_ASSERT_EQUAL_INT(0, byte_regex_validate_pattern("\xFF\x0F\x01\xF1"), __LINE__, "Pattern should be OK (pattern = [0-9A-F]{34}F1)");
    UNITY_TEST_ASSERT_EQUAL_INT(0, byte_regex_validate_pattern("\x0F\x01\xF1"), __LINE__, "Pattern should be OK (pattern = [0-9A-F]{2}F1)");
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    // RUN_TEST(test_compile_pattern_0);
    RUN_TEST(test_validate_pattern);
    return UNITY_END();
}

/**
  * For native dev-platform or for some embedded frameworks
  */
int main(void) {
    return runUnityTests();
}

// /**
//   * For Arduino framework
//   */
// void setup()
// {
//     // Wait ~2 seconds before the Unity test runner
//     // establishes connection with a board Serial interface
//     delay(2000);

//     runUnityTests();
// }
// void loop() {}
