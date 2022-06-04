#include "../src/main.h"
#include <unity.h>
#include "test_pin_api.h"
#include <cstdio>

void setUp(void) {
}
void tearDown(void) {
}


int main() {
  HAL_Init();  // initialize the HAL library
  HAL_Delay(1000);

  UNITY_BEGIN();
  RUN_TEST(test_port_lookup);
  RUN_TEST(test_num_lookup);
  RUN_TEST(test_pin_is_available);
  RUN_TEST(test_pin_io);
  RUN_TEST(test_pin_toggle);
  RUN_TEST(test_multiple_pins);

  UNITY_END();  // stop unit testing

  while (1) {
  }
}

#ifdef __cplusplus
extern "C" {
#endif

void SysTick_Handler(void) {
  HAL_IncTick();
}

void Error_Handler() {
  while (1)
    ;
}

void assert_failed(uint8_t* file, uint32_t line) {
  char buff[40];
  snprintf(buff, 40, "ASSERT: %s  :: %d", file, static_cast<int>(line));
  TEST_MESSAGE(buff);
  return;
}

#ifdef __cplusplus
}
#endif
