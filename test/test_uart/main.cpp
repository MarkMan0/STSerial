/**
 * @file main.cpp
 * @brief tests for UART_DMA
 */

#include "../../src/main.h"
#include "../../src/uart.h"
#include "../../src/stm32f3xx_it.h"
#include <unity.h>


UART_DMA uart2(UART_DMA::uart2_hw_init, UART_DMA::uart2_enable_isrs);
UART_DMA uart1(UART_DMA::uart1_hw_init, UART_DMA::uart1_enable_isrs);


void setUp() {
}
void tearDown() {
  uart1.flush();
  HAL_Delay(500);
  while (uart1.available()) {
    uart1.get_one();
  }
  uart1.reset_buffers();
}

const char msg[] = "Hello world!";
const int msg_len = sizeof(msg) - 1;


void test_send_buffer() {
  uart1.send("Hello world!");
  HAL_Delay(100);
  TEST_ASSERT_EQUAL(0, uart1.available());

  uart1.tick();
  HAL_Delay(100);
  TEST_ASSERT_EQUAL(msg_len, uart1.available());

  char rec[30]{};
  for (int i = 0; i < 30 && uart1.available(); ++i) {
    rec[i] = uart1.get_one();
  }
  TEST_ASSERT_EQUAL_STRING(msg, rec);
}

void test_transmit_immediate() {
  uart1.transmit(msg);
  HAL_Delay(100);
  TEST_ASSERT_EQUAL(msg_len, uart1.available());
  char rec[30]{};
  for (int i = 0; i < 30 && uart1.available(); ++i) {
    rec[i] = uart1.get_one();
  }
  TEST_ASSERT_EQUAL_STRING(msg, rec);
}


struct S {
  int a;
  bool b;
  float c;
};

void test_transmit_data() {
  S s1{ 1, false, .1 };

  uart1.transmit(&s1, sizeof(s1));
  HAL_Delay(500);

  TEST_ASSERT_EQUAL(sizeof(s1), uart1.available());
  uint8_t rec[30];
  for (int i = 0; i < 30 && uart1.available(); ++i) {
    rec[i] = uart1.get_one();
  }

  S s2 = *reinterpret_cast<S*>(rec);

  TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, sizeof(s1));
}


void test_send_data() {
  S s1{ 568, true, -3.14 };

  uart1.send(&s1, sizeof(s1));
  uart1.flush();
  HAL_Delay(500);

  TEST_ASSERT_EQUAL(sizeof(s1), uart1.available());
  uint8_t rec[30];
  for (int i = 0; i < 30 && uart1.available(); ++i) {
    rec[i] = uart1.get_one();
  }

  S s2 = *reinterpret_cast<S*>(rec);

  TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, sizeof(s1));
}

void test_printf() {
  const char fmt[] = "Hi %s %d %e";
  const char str[] = "Joe";
  const int i = -434;
  const float f = 3.1415;
  char expected[50];
  int len = snprintf(expected, 50, fmt, str, i, f);

  int len2 = uart1.printf(fmt, str, i, f);
  uart1.flush();
  HAL_Delay(1000);
  char result[50]{};
  for (int i = 0; i < 50 && uart1.available(); ++i) {
    result[i] = uart1.get_one();
  }

  TEST_ASSERT_NOT_EQUAL(0, len2);
  TEST_ASSERT_EQUAL(len, len2);
  TEST_ASSERT_EQUAL_STRING(expected, result);
}


int main() {
  HAL_Init();

  HAL_Delay(500);
  uart1.begin(115200);
  UNITY_BEGIN();

  RUN_TEST(test_send_buffer);
  RUN_TEST(test_transmit_immediate);
  RUN_TEST(test_transmit_data);
  RUN_TEST(test_send_data);
  RUN_TEST(test_printf);

  HAL_Delay(500);
  UNITY_END();
  while (1) {
  }
}



void DMA1_Channel4_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart1.hdmatx_);
}

void DMA1_Channel5_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart1.hdmarx_);
}

void DMA1_Channel6_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart2.hdmarx_);
}

void DMA1_Channel7_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart2.hdmatx_);
}

void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&uart1.huart_);
}

void USART2_IRQHandler(void) {
  HAL_UART_IRQHandler(&uart2.huart_);
}

#include "../../src/uart.cpp"
#include "../../src/uart_msp_impl.cpp"
