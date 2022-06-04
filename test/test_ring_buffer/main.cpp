#include "../../src/main.h"
#include "../../src/ring_buffer.h"
#include <unity.h>

void setUp() {
}
void tearDown() {
}

void test_buffer_create() {
  RingBuffer<uint8_t, 10> buffer;

  TEST_ASSERT_TRUE(buffer.is_empty());
  TEST_ASSERT_FALSE(buffer.is_full());
  TEST_ASSERT_EQUAL(10, buffer.get_num_free());
}


void test_buffer_read_write() {
  RingBuffer<uint8_t, 10> buffer;

  buffer.push(10);
  buffer.push(20);
  TEST_ASSERT_FALSE(buffer.is_empty());
  TEST_ASSERT_FALSE(buffer.is_full());
  TEST_ASSERT_EQUAL(8, buffer.get_num_free());
  TEST_ASSERT_EQUAL(10, buffer.pop());
  TEST_ASSERT_EQUAL(20, buffer.pop());

  TEST_ASSERT_TRUE(buffer.is_empty());
  TEST_ASSERT_FALSE(buffer.is_full());
}

void test_is_full() {
  RingBuffer<uint8_t, 2> buff;
  buff.push(1);
  buff.push(2);
  TEST_ASSERT_TRUE(buff.is_full());
  buff.pop();
  TEST_ASSERT_FALSE(buff.is_full());
}

void test_is_empty() {
  RingBuffer<uint8_t, 2> buff;

  TEST_ASSERT_TRUE(buff.is_empty());
  buff.push(1);
  TEST_ASSERT_FALSE(buff.is_empty());
  buff.push(2);
  TEST_ASSERT_FALSE(buff.is_empty());
  buff.pop();
  TEST_ASSERT_FALSE(buff.is_empty());
  buff.pop();
  TEST_ASSERT_TRUE(buff.is_empty());
}

void test_overflow() {
  RingBuffer<uint8_t, 3> buff;

  buff.push(0);
  buff.push(1);
  buff.push(2);

  TEST_ASSERT_EQUAL(0, buff.pop());
  buff.push(3);
  TEST_ASSERT_TRUE(buff.is_full());
  TEST_ASSERT_EQUAL(1, buff.pop());
  TEST_ASSERT_EQUAL(2, buff.pop());
  TEST_ASSERT_EQUAL(3, buff.pop());
  TEST_ASSERT_TRUE(buff.is_empty());

  buff.push(4);
  buff.push(5);
  TEST_ASSERT_EQUAL(4, buff.pop());
  TEST_ASSERT_EQUAL(5, buff.pop());
  TEST_ASSERT_TRUE(buff.is_empty());
}


void test_peek() {
  RingBuffer<uint8_t, 4> buff;

  buff.push(0);
  buff.push(1);
  TEST_ASSERT_EQUAL(0, buff.peek());
  TEST_ASSERT_EQUAL(0, buff.pop());
  TEST_ASSERT_EQUAL(1, buff.peek());
}


void test_num_free_occupied() {
  RingBuffer<uint8_t, 5> buff;

  TEST_ASSERT_EQUAL(5, buff.get_num_free());
  TEST_ASSERT_EQUAL(0, buff.get_num_occupied());

  buff.push(1);
  TEST_ASSERT_EQUAL(4, buff.get_num_free());
  TEST_ASSERT_EQUAL(1, buff.get_num_occupied());


  buff.push(2);
  TEST_ASSERT_EQUAL(3, buff.get_num_free());
  TEST_ASSERT_EQUAL(2, buff.get_num_occupied());

  buff.pop();
  TEST_ASSERT_EQUAL(4, buff.get_num_free());
  TEST_ASSERT_EQUAL(1, buff.get_num_occupied());


  buff.pop();
  TEST_ASSERT_EQUAL(5, buff.get_num_free());
  TEST_ASSERT_EQUAL(0, buff.get_num_occupied());

  buff.push(0);
  buff.push(1);
  buff.push(2);
  buff.push(3);
  buff.push(4);
  TEST_ASSERT_EQUAL(0, buff.get_num_free());
  TEST_ASSERT_EQUAL(5, buff.get_num_occupied());
  buff.push(5);
  TEST_ASSERT_EQUAL(0, buff.get_num_free());
  TEST_ASSERT_EQUAL(5, buff.get_num_occupied());
}



int main() {
  HAL_Init();
  HAL_Delay(500);

  UNITY_BEGIN();

  RUN_TEST(test_buffer_create);
  RUN_TEST(test_buffer_read_write);
  RUN_TEST(test_is_full);
  RUN_TEST(test_is_empty);
  RUN_TEST(test_overflow);
  RUN_TEST(test_peek);
  RUN_TEST(test_num_free_occupied);

  HAL_Delay(500);
  UNITY_END();

  while (1) {
  }
}
