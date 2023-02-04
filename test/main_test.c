#include "unity.h"

#ifndef ENVIRONMENT_NATIVE
#include <Arduino.h>
#endif //ENVIRONMENT_NATIVE

#define TEST_PHASE(run_func) do{run_func();test_phase++;}while(0)

void byte_regex_run_tests();
void byte_regex_setup();
void byte_regex_teardown();

#ifndef ENVIRONMENT_NATIVE
void spi_debugger_run_tests();
void spi_debugger_setup();
void spi_debugger_teardown();
#else
void spi_debugger_run_tests() {}
void spi_debugger_setup() {}
void spi_debugger_teardown() {}
#endif //ENVIRONMENT_NATIVE

unsigned int test_phase = 0;

void setUp(void)
{
    switch (test_phase) {
        case 0:
            byte_regex_setup();
            break;

        case 1:
            spi_debugger_setup();
            break;

        default:
            break;
    }
}

void tearDown(void)
{
    switch (test_phase) {
        case 0:
            byte_regex_teardown();
            break;

        case 1:
            spi_debugger_teardown();
            break;

        default:
            break;
    }
}

int run_unity_tests(void)
{
    UNITY_BEGIN();
    TEST_PHASE(byte_regex_run_tests);
    TEST_PHASE(spi_debugger_run_tests);
    return UNITY_END();
}

/**
  * For native dev-platform or for some embedded frameworks
  */
int main(void) {
    return run_unity_tests();
}

/**
  * For Arduino framework
  */
void setup()
{
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    #ifndef ENVIRONMENT_NATIVE
    delay(2000);
    printf("hello\n");
    #endif //ENVIRONMENT_NATIVE
    
    run_unity_tests();
}
void loop() {}