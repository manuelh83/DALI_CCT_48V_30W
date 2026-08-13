#ifndef UNITY_H
#define UNITY_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_ASSERT_TRUE(condition) do { if (!(condition)) { fprintf(stderr, "Assertion failed: %s:%d\n", __FILE__, __LINE__); exit(1); } } while (0)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT_TRUE(!(condition))
#define TEST_ASSERT_EQUAL_UINT(expected, actual) TEST_ASSERT_TRUE((unsigned)(expected) == (unsigned)(actual))
#define TEST_ASSERT_EQUAL_HEX8(expected, actual) TEST_ASSERT_TRUE((unsigned)(expected) == (unsigned)(actual))
#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual) TEST_ASSERT_TRUE(fabs((expected) - (actual)) <= (delta))

#endif
