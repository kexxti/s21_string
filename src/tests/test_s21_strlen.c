
#include "test_s21_strlen.h"

// Тест 1: пустая строка
START_TEST(test_strlen_empty_string) {
  char s21_str[] = "";
  char std_str[] = "";
  ck_assert_int_eq(s21_strlen(s21_str), strlen(std_str));
}
END_TEST

// Тест 2: небольшая строка
START_TEST(test_strlen_usual_string) {
  char s21_str[] = "Hello";
  char std_str[] = "Hello";
  ck_assert_int_eq(s21_strlen(s21_str), strlen(std_str));
}
END_TEST

// Тест 3: cтрока с пробелами и символами
START_TEST(test_strlen_with_spaces) {
  char s21_str[] = "\tHello world!\n";
  char std_str[] = "\tHello world!\n";
  ck_assert_int_eq(s21_strlen(s21_str), strlen(std_str));
}
END_TEST

// Тест 4: кириллица
START_TEST(test_strlen_rus) {
  char s21_str[] = "Я пошел в школу 21";
  char std_str[] = "Я пошел в школу 21";
  ck_assert_int_eq(s21_strlen(s21_str), strlen(std_str));
}
END_TEST

// Тест 5: длинная строка
START_TEST(test_strlen_long_string) {
  char s21_str[] =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
      "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
  char std_str[] =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
      "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.";
  ck_assert_int_eq(s21_strlen(s21_str), strlen(std_str));
}
END_TEST

Suite *s21_strlen_suite(void) {
  Suite *s = suite_create("s21_strlen");
  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_strlen_empty_string);
  tcase_add_test(tc_core, test_strlen_usual_string);
  tcase_add_test(tc_core, test_strlen_with_spaces);
  tcase_add_test(tc_core, test_strlen_rus);
  tcase_add_test(tc_core, test_strlen_long_string);
  suite_add_tcase(s, tc_core);
  return s;
}
