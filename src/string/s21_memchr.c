#include "s21_string.h"

void* s21_memchr(const void* str, int c, s21_size_t n) {
  s21_size_t len = 0;
  unsigned char* mem = (unsigned char*)str;
  void* result = S21_NULL;
  if (n != 0) {
    while (*mem != (unsigned char)c && len < n) {
      len++;
      mem++;
    }
    if (*mem == (unsigned char)c) {
      result = mem;
    }
  }
  return result;
}
