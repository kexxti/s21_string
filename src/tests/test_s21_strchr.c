#include "test_s21_strchr.h"

#include "../string/s21_string.h"

// Тест 1: поиск 'H' в "Hello"
START_TEST(test_strchr_char_in_beginning) {
  char str[] = "Hello";
  int c = 'H';
  char *res_s21 = s21_strchr(str, c);
  char *res_std = strchr(str, c);
  ck_assert_ptr_eq(res_s21, res_std);
}
END_TEST

// Тест 2: 'o' в середине предложения
START_TEST(test_strchr_char_in_middle) {
  char str[] = "Hello, world!";
  int c = 'o';
  char *res_s21 = s21_strchr(str, c);
  char *res_std = strchr(str, c);
  ck_assert_ptr_eq(res_s21, res_std);
}
END_TEST

// Тест 3: 'o' последний символ
START_TEST(test_strchr_char_in_end) {
  char str[] =
      "rldrldrldrldrldrldrldrldrlrldrldrldrldrldrldrldrldrldrldrldrldrldrldrldr"
      "ldrldrldrldrldHello";
  int c = 'o';
  char *res_s21 = s21_strchr(str, c);
  char *res_std = strchr(str, c);
  ck_assert_ptr_eq(res_s21, res_std);
}
END_TEST

// Тест 4: поиск символа, которого нет в строке
START_TEST(test_strchr_char_not_found) {
  char str[] = "Hello";
  int c = 'X';
  char *res_s21 = s21_strchr(str, c);
  char *res_std = strchr(str, c);
  ck_assert_ptr_eq(res_s21, res_std);
}
END_TEST

// Тест 5 поиск '\0'
START_TEST(test_strchr_end_od_string) {
  char str[] =
      "rldrldrldrldrldrldrldrldrlrldrldrldrldrldrldrldrldrldrldrldrldrldrldrldr"
      "ldrldrldrldrldHello";
  int c = '\0';
  char *res_s21 = s21_strchr(str, c);
  char *res_std = strchr(str, c);
  ck_assert_ptr_eq(res_s21, res_std);
}
END_TEST

// Тест 6: пустая строка, должен вернутся NULL
START_TEST(test_strchr_empty_string) {
  char str[] = "";
  int c = 'H';
  char *res_s21 = s21_strchr(str, c);
  char *res_std = strchr(str, c);
  ck_assert_ptr_eq(res_s21, res_std);
}
END_TEST

// Тест 7: пустая строка, поиск конца строки
START_TEST(test_strchr_empty_string_end_of_string) {
  char str[] = "";
  int c = '\0';
  char *res_s21 = s21_strchr(str, c);
  char *res_std = strchr(str, c);
  ck_assert_ptr_eq(res_s21, res_std);
}
END_TEST

// Тест 7: пустая строка, поиск символа другого регистра, должен вернутся NULL
START_TEST(test_strchr_lower_case) {
  char str[] = "Hello";
  int c = 'h';
  char *res_s21 = s21_strchr(str, c);
  char *res_std = strchr(str, c);
  ck_assert_ptr_eq(res_s21, res_std);
}
END_TEST

Suite *s21_strchr_suite(void) {
  Suite *s = suite_create("s21_strchr");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_strchr_char_in_beginning);
  tcase_add_test(tc_core, test_strchr_char_in_middle);
  tcase_add_test(tc_core, test_strchr_char_in_end);
  tcase_add_test(tc_core, test_strchr_char_not_found);
  tcase_add_test(tc_core, test_strchr_end_od_string);
  tcase_add_test(tc_core, test_strchr_empty_string);
  tcase_add_test(tc_core, test_strchr_empty_string_end_of_string);
  tcase_add_test(tc_core, test_strchr_lower_case);
  suite_add_tcase(s, tc_core);
  return s;
}
