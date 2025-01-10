#include "s21_string.h"

s21_size_t s21_strspn(const char *str1, const char *str2) {
  s21_size_t count = 0;
  while (*str1) {
    int found_flag = 0;
    const char *s = str2;

    while (*s) {
      if (*str1 == *s) {
        found_flag = 1;
        break;
      }
      s++;
    }

    if (!found_flag) {
      break;
    }

    count++;
    str1++;
  }

  return count;
}