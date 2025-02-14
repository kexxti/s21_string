#include "s21_string.h"

char *s21_strtok(char *str, const char *delim) {
  static char *olds = S21_NULL;
  char *result = S21_NULL;

  if (delim == S21_NULL) {
    olds = S21_NULL;
  } else {
    if (str == S21_NULL) {
      if (olds == S21_NULL) return S21_NULL;  // Если старой строки нет, выходим
      str = olds;
    }

    str += s21_strspn(str, delim);

    if (*str != '\0') {
      char *end = str + s21_strcspn(str, delim);

      if (*end != '\0') {
        *end = '\0';
        olds = end + 1;
      } else {
        olds = S21_NULL;
      }

      result = str;
    } else {
      olds = S21_NULL;
    }
  }

  return result;
}
