#include "s21_string.h"

s21_size_t s21_strrspn(const char *str1, const char *str2) {
  s21_size_t count = 0;
  const char *end = str1 + s21_strlen(str1);

  while (end > str1) {
    end--;
    int found_flag = 0;
    const char *s = str2;

    while (*s) {
      if (*end == *s) {
        found_flag = 1;
        break;
      }
      s++;
    }

    if (!found_flag) {
      break;
    }

    count++;
  }

  return count;
}