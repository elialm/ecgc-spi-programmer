#ifndef ENVIRONMENT_NATIVE

#include "unity.h"
#include "spi_debugger.c"
#include "pin.h"

const uint8_t *test_data = "\xc3[B>\xfa\xaf\xcb&\x1a\xaa\x98@\x03\xf5\x9a\x05\xf4=";
pin_t ss_pin;
pin_t dbg_pin;

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    spi_debugger_deinit(&ss_pin, &dbg_pin);
}

void test_spi_debugger_init()
{
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_init(&ss_pin, &dbg_pin), __LINE__, "");
}

void test_spi_debugger_auto_increment_enable()
{
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_init(&ss_pin, &dbg_pin), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_set_auto_increment(&ss_pin, &dbg_pin, true), __LINE__, "");
}

void test_spi_debugger_auto_increment_disable()
{
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_init(&ss_pin, &dbg_pin), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_set_auto_increment(&ss_pin, &dbg_pin, false), __LINE__, "");
}

void test_spi_debugger_set_address()
{
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_init(&ss_pin, &dbg_pin), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_set_address(&ss_pin, &dbg_pin, 0x4000), __LINE__, "");
}

void test_spi_debugger_write_read_only_bursts()
{
    const size_t data_length = 16;
    uint8_t read_buffer[data_length];

    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_init(&ss_pin, &dbg_pin), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_set_auto_increment(&ss_pin, &dbg_pin, true), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_set_address(&ss_pin, &dbg_pin, 0x4000), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_write(&ss_pin, &dbg_pin, test_data, data_length), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_read(&ss_pin, &dbg_pin, read_buffer, data_length), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_UINT8_ARRAY(test_data, read_buffer, data_length, __LINE__, "");
}

void test_spi_debugger_write_read_only_individual()
{
    const size_t data_length = 15;
    uint8_t read_buffer[data_length];

    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_init(&ss_pin, &dbg_pin), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_set_auto_increment(&ss_pin, &dbg_pin, true), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_set_address(&ss_pin, &dbg_pin, 0x4000), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_write(&ss_pin, &dbg_pin, test_data, data_length), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_read(&ss_pin, &dbg_pin, read_buffer, data_length), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_UINT8_ARRAY(test_data, read_buffer, data_length, __LINE__, "");
}

void test_spi_debugger_write_read_individual_and_burst()
{
    const size_t data_length = 18;
    uint8_t read_buffer[data_length];

    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_init(&ss_pin, &dbg_pin), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_set_auto_increment(&ss_pin, &dbg_pin, true), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_set_address(&ss_pin, &dbg_pin, 0x4000), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_write(&ss_pin, &dbg_pin, test_data, data_length), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_INT8(0, spi_debugger_read(&ss_pin, &dbg_pin, read_buffer, data_length), __LINE__, "");
    UNITY_TEST_ASSERT_EQUAL_UINT8_ARRAY(test_data, read_buffer, data_length, __LINE__, "");
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_spi_debugger_init);
    RUN_TEST(test_spi_debugger_auto_increment_enable);
    RUN_TEST(test_spi_debugger_auto_increment_disable);
    RUN_TEST(test_spi_debugger_set_address);
    RUN_TEST(test_spi_debugger_write_read_only_bursts);
    RUN_TEST(test_spi_debugger_write_read_only_individual);
    RUN_TEST(test_spi_debugger_write_read_individual_and_burst);
    return UNITY_END();
}

/**
  * For Arduino framework
  */
void setup()
{
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    delay(2000);

    ss_pin = pin_define(B, 2);
    dbg_pin = pin_define(C, 0);

    runUnityTests();
}
void loop() {}

#endif //ENVIRONMENT_NATIVE