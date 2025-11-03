#include "unity.h"
#include "string.h"
#include <stdio.h>
#include "ring_buffer.h"

void setUp(void) {}
void tearDown() {}

TEST_CASE(10, 20, 22)
TEST_CASE(0, 0, 32)
TEST_CASE(10, 9, 0)
void test_buffer_free(int tail, int head, int expected) {
  RingBuffer rb = {0};
  rb.tail = tail;
  rb.head = head;

  TEST_ASSERT_EQUAL_INT(rb_free(&rb), expected);
}

struct TestRingBuffer {
  RingBuffer buffer;
  char* str;
  int expected_head;
};

void test_buffer_write_array() {
  struct TestRingBuffer test_rb[] = {
    {.buffer = {0}, "test", 4},
    {.buffer = {0}, "testtesttesttesttesttesttesttes", 31},
    {.buffer = {.elements = {0}, 24, 24}, "testtest", 0},
    {.buffer = {.elements = {0}, 24, 24}, "testtesttest", 4}
  };

  for (size_t i = 0; i < sizeof(test_rb) / sizeof(struct TestRingBuffer); i++){
    struct TestRingBuffer buf = test_rb[i];
    int status = rb_write_array(&buf.buffer, buf.str, strlen(buf.str));
    TEST_ASSERT_EQUAL_INT(1, status);
    TEST_ASSERT_EQUAL_INT(buf.expected_head, buf.buffer.head);  
  }
}

void test_buffer_write_array_big() {
  RingBuffer rb = {0};
  char str[35];
  memset(str, 'a', sizeof(str));
  TEST_ASSERT_EQUAL_INT(0, rb_write_array(&rb, str, strlen(str)));
}