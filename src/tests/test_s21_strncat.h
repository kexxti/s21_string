#ifndef TEST_S21_STRNCAT_H
#define TEST_S21_STRNCAT_H

#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// функция, сбрасывает/копирует строки
static void reset_buffers(char *dest_s21, char *dest_std,
                          const char *initial_dest, const char **src_ptr,
                          const char *src_str) {
  strcpy(dest_s21, initial_dest);
  strcpy(dest_std, initial_dest);
  *src_ptr = src_str;
}

#endif
