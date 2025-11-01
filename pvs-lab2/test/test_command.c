#include "unity.h"

void setUp(void) {}
void tearDown() {}

int max(int* array, int len) {
  int max = INT32_MIN;
  for (size_t i = 0; i < len; i++){
    max = array[i] > max ? array[i] : max;
  }
  return max;
}

int arr1[] = {1,2,3,1};
int arr2[] = {0,0,0,-1};

TEST_CASE(arr1, 3)
TEST_CASE(arr2, 0)
void test_context(int* x, int expected) {
  int actual = max(x, 5);
  TEST_ASSERT_EQUAL_INT(expected, actual);
}