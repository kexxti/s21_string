#include "test_s21_sprintf.h"

#include <check.h>
#include <stdio.h>

#include "../string/s21_sprintf.h"

START_TEST(test_sprintf_basic_integers) {
  char buf1[256], buf2[256];
  int a = 42;
  sprintf(buf1, "%d", a);
  s21_sprintf(buf2, "%d", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_negative_integers) {
  char buf1[256], buf2[256];
  int a = -42;
  sprintf(buf1, "%d", a);
  s21_sprintf(buf2, "%d", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_width) {
  char buf1[256], buf2[256];
  int a = 123;
  sprintf(buf1, "[%5d]", a);
  s21_sprintf(buf2, "[%5d]", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_precision_integer) {
  char buf1[256], buf2[256];
  int a = 123;
  sprintf(buf1, "[%.5d]", a);
  s21_sprintf(buf2, "[%.5d]", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_left_align) {
  char buf1[256], buf2[256];
  int a = 123;
  sprintf(buf1, "[%-5d]", a);
  s21_sprintf(buf2, "[%-5d]", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_zero_padding) {
  char buf1[256], buf2[256];
  int a = 123;
  sprintf(buf1, "[%05d]", a);
  s21_sprintf(buf2, "[%05d]", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_plus_flag) {
  char buf1[256], buf2[256];
  int a = 123;
  sprintf(buf1, "[%+d]", a);
  s21_sprintf(buf2, "[%+d]", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_space_flag) {
  char buf1[256], buf2[256];
  int a = 123;
  sprintf(buf1, "[% d]", a);
  s21_sprintf(buf2, "[% d]", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_octal_alternative) {
  char buf1[256], buf2[256];
  unsigned int a = 123;
  sprintf(buf1, "[%#o]", a);
  s21_sprintf(buf2, "[%#o]", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_hex_alternative_lower) {
  char buf1[256], buf2[256];
  unsigned int a = 0x1a3f;
  sprintf(buf1, "[%#x]", a);
  s21_sprintf(buf2, "[%#x]", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_hex_alternative_upper) {
  char buf1[256], buf2[256];
  unsigned int a = 0x1a3f;
  sprintf(buf1, "[%#X]", a);
  s21_sprintf(buf2, "[%#X]", a);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_char) {
  char buf1[256], buf2[256];
  char c = 'A';
  sprintf(buf1, "[%c]", c);
  s21_sprintf(buf2, "[%c]", c);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_string) {
  char buf1[256], buf2[256];
  char *s = "Hello, world!";
  sprintf(buf1, "[%s]", s);
  s21_sprintf(buf2, "[%s]", s);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_string_precision) {
  char buf1[256], buf2[256];
  char *s = "Hello, world!";
  sprintf(buf1, "[%.5s]", s);
  s21_sprintf(buf2, "[%.5s]", s);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_pointer) {
  char buf1[256], buf2[256];
  void *p = (void *)0xDEADBEEF;
  sprintf(buf1, "[%p]", p);
  s21_sprintf(buf2, "[%p]", p);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_percent) {
  char buf1[256], buf2[256];
  sprintf(buf1, "100%% sure");
  s21_sprintf(buf2, "100%% sure");
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_float) {
  char buf1[256], buf2[256];
  float f = 123.456f;
  sprintf(buf1, "[%f]", f);
  s21_sprintf(buf2, "[%f]", f);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_float_precision) {
  char buf1[256], buf2[256];
  double d = 3.141592653589793;
  sprintf(buf1, "[%.2f]", d);
  s21_sprintf(buf2, "[%.2f]", d);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_exponential) {
  char buf1[256], buf2[256];
  double d = 0.00012345;
  sprintf(buf1, "[%e]", d);
  s21_sprintf(buf2, "[%e]", d);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_scientific_upper) {
  char buf1[256], buf2[256];
  double d = 123456.789;
  sprintf(buf1, "[%E]", d);
  s21_sprintf(buf2, "[%E]", d);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_general) {
  char buf1[256], buf2[256];
  double d = 123.456;
  sprintf(buf1, "[%g]", d);
  s21_sprintf(buf2, "[%g]", d);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_general_upper) {
  char buf1[256], buf2[256];
  double d = 123.456;
  sprintf(buf1, "[%G]", d);
  s21_sprintf(buf2, "[%G]", d);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_dynamic_width_precision) {
  char buf1[256], buf2[256];
  int width = 10, prec = 4;
  double d = 3.141592653589793;
  sprintf(buf1, "[%*.*f]", width, prec, d);
  s21_sprintf(buf2, "[%*.*f]", width, prec, d);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_multiple_specifiers) {
  char buf1[256], buf2[256];
  int a = -42;
  unsigned int u = 100;
  char c = 'Z';
  char *s = "Test";
  double d = 2.71828;
  sprintf(buf1,
          "Int: [%d], Unsigned: [%u], Char: [%c], String: [%s], Float: [%.3f]",
          a, u, c, s, d);
  s21_sprintf(
      buf2,
      "Int: [%d], Unsigned: [%u], Char: [%c], String: [%s], Float: [%.3f]", a,
      u, c, s, d);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_empty_format) {
  char buf1[256], buf2[256];
  sprintf(buf1, "");
  s21_sprintf(buf2, "");
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_literal_only) {
  char buf1[256], buf2[256];
  sprintf(buf1, "Just a literal string, no conversions.");
  s21_sprintf(buf2, "Just a literal string, no conversions.");
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_null_string) {
  char buf1[256], buf2[256];
  char *null_str = NULL;
  sprintf(buf1, "[%s]", null_str);
  s21_sprintf(buf2, "[%s]", null_str);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_long_long) {
  char buf1[256], buf2[256];
  long long ll = -9223372036854775807LL;
  sprintf(buf1, "[%lld]", ll);
  s21_sprintf(buf2, "[%lld]", ll);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_unsigned_long_long) {
  char buf1[256], buf2[256];
  unsigned long long ull = 18446744073709551615ULL;
  sprintf(buf1, "[%llu]", ull);
  s21_sprintf(buf2, "[%llu]", ull);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

START_TEST(test_sprintf_float_alternative) {
  char buf1[256], buf2[256];
  double d = 12.0;
  sprintf(buf1, "[%#.0f]", d);
  s21_sprintf(buf2, "[%#.0f]", d);
  ck_assert_str_eq(buf1, buf2);
}
END_TEST

Suite *s21_sprintf_suite(void) {
  Suite *s = suite_create("s21_sprintf");
  TCase *tc = tcase_create("Core");

  tcase_add_test(tc, test_sprintf_basic_integers);
  tcase_add_test(tc, test_sprintf_negative_integers);
  tcase_add_test(tc, test_sprintf_width);
  tcase_add_test(tc, test_sprintf_precision_integer);
  tcase_add_test(tc, test_sprintf_left_align);
  tcase_add_test(tc, test_sprintf_zero_padding);
  tcase_add_test(tc, test_sprintf_plus_flag);
  tcase_add_test(tc, test_sprintf_space_flag);
  tcase_add_test(tc, test_sprintf_octal_alternative);
  tcase_add_test(tc, test_sprintf_hex_alternative_lower);
  tcase_add_test(tc, test_sprintf_hex_alternative_upper);
  tcase_add_test(tc, test_sprintf_char);
  tcase_add_test(tc, test_sprintf_string);
  tcase_add_test(tc, test_sprintf_string_precision);
  tcase_add_test(tc, test_sprintf_pointer);
  tcase_add_test(tc, test_sprintf_percent);
  tcase_add_test(tc, test_sprintf_float);
  tcase_add_test(tc, test_sprintf_float_precision);
  tcase_add_test(tc, test_sprintf_exponential);
  tcase_add_test(tc, test_sprintf_scientific_upper);
  tcase_add_test(tc, test_sprintf_general);
  tcase_add_test(tc, test_sprintf_general_upper);
  tcase_add_test(tc, test_sprintf_dynamic_width_precision);
  tcase_add_test(tc, test_sprintf_multiple_specifiers);
  tcase_add_test(tc, test_sprintf_empty_format);
  tcase_add_test(tc, test_sprintf_literal_only);
  tcase_add_test(tc, test_sprintf_null_string);
  tcase_add_test(tc, test_sprintf_long_long);
  tcase_add_test(tc, test_sprintf_unsigned_long_long);
  tcase_add_test(tc, test_sprintf_float_alternative);

  suite_add_tcase(s, tc);
  return s;
}
