#include "s21_string.h"

char *s21_strtok(char *str, const char *delim) {
  static char *olds;
  char *end;
  if (str == S21_NULL) {
    str = olds;
  }

  if (*str == '\0') {
    olds = str;
  } else {
    str += s21_strspn(str, delim);
    if (*str == '\0') {
      olds = str;
    } else {
      end = str + s21_strcspn(str, delim);
      if (*end == '\0') {
        olds = end;
      }
    }
  }

  *end = '\0';
  olds = end + 1;
  return *str == '\0' ? S21_NULL : str;
}