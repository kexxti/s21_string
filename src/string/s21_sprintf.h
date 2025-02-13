#ifndef S21_SPRINTF_H
#define S21_SPRINTF_H

#include <stdarg.h>
#include <stdbool.h>

#include "s21_string.h"

typedef enum PRINT_MODIFIER {
  ERR,
  CHAR,
  DECIMAL,
  INTEGER,
  FLOAT,
  UOCTAL,
  STRING,
  UDECIMAL,
  UHEX,
  POINTER,
  NUMBER,
  PERCENT
} PRINT_MODIFIER;

typedef enum PRINT_LENGTH { NONE, SHORT, LONG, EXTENDED_DOUBLE } PRINT_LENGTH;

typedef struct print_format {
  bool is_symbol;        // Если нет спецификатора - просто символ для вывода
  char matching_symbol;  // Если is_symbol==true, этот символ выводится

  bool left_align;  // Флаг '-'
  bool force_sign;  // Флаг '+'
  bool space;       // Флаг ' ' (пробел)
  bool alt_form;    // Флаг '#'
  bool zero_pad;    // Флаг '0'

  s21_size_t width;  // Минимальная ширина поля
  bool precision_specified;
  s21_size_t precision;  // Значение точности

  PRINT_LENGTH length_modifier;  // Модификатор длины (h, l, L)
  PRINT_MODIFIER type_modifier;  // Тип спецификатора
  char
      spec;  // Фактический символ спецификатора (например, 'f', 'e', 'g', etc.)
} print_format;

int s21_sprintf(char *str, const char *format, ...);
print_format *s21_sprintf_init_format();
void s21_sprintf_fill_format_default(print_format *cf);
void s21_sprintf_read_format(const char **format, print_format *cf,
                             va_list args);
s21_size_t s21_sprintf_get_width(const char **format, va_list args);
s21_size_t s21_sprintf_get_precision(const char **format, va_list args);
void s21_sprintf_normalize_format(print_format *cf);
void s21_sprintf_apply_format(char **dest, print_format *cf, va_list args,
                              int *written);

/* Прототипы вспомогательных функций форматирования */
int s21_sprintf_format_char(va_list args, char *buffer);
int s21_sprintf_format_string(print_format *cf, va_list args, char *buffer);
int s21_sprintf_format_decimal(print_format *cf, va_list args, char *buffer);
int s21_sprintf_format_unsigned(print_format *cf, va_list args, char *buffer,
                                int base, bool uppercase);
int s21_sprintf_format_float(print_format *cf, va_list args, char *buffer);
int s21_sprintf_format_float_sci(double val, int prec, char *buffer,
                                 bool uppercase);
int s21_sprintf_format_pointer(va_list args, char *buffer);
void s21_sprintf_write_padding(char **dest, int pad, char pad_char,
                               int *written);

#endif  // S21_SPRINTF_H
