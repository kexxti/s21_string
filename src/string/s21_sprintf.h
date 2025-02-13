#ifndef S21_SPRINTF_H
#define S21_SPRINTF_H

#include <stdarg.h>
#include <stdbool.h>

#include "s21_string.h"

typedef enum MODIFIER {
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
} MODIFIER;

typedef enum LENGTH { NONE, SHORT, LONG, EXTENDED_DOUBLE } LENGTH;

// Структура current_format (имя оставлено, как в s21_sscanf)
typedef struct current_format {
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

  LENGTH length_modifier;  // Модификатор длины (h, l, L)
  MODIFIER type_modifier;  // Тип спецификатора
  char
      spec;  // Фактический символ спецификатора (например, 'f', 'e', 'g', etc.)
} current_format;

int s21_sprintf(char *str, const char *format, ...);
current_format *s21_sprintf_init_format();
void s21_sprintf_fill_format_default(current_format *cf);
void s21_sprintf_read_format(const char **format, current_format *cf,
                             va_list args);
s21_size_t s21_sprintf_get_width(const char **format, va_list args);
s21_size_t s21_sprintf_get_precision(const char **format, va_list args);
void s21_sprintf_normalize_format(current_format *cf);
void s21_sprintf_apply_format(char **dest, current_format *cf, va_list args,
                              int *written);

/* Прототипы вспомогательных функций форматирования */
int s21_sprintf_format_char(va_list args, char *buffer);
int s21_sprintf_format_string(current_format *cf, va_list args, char *buffer);
int s21_sprintf_format_decimal(current_format *cf, va_list args, char *buffer);
int s21_sprintf_format_unsigned(current_format *cf, va_list args, char *buffer,
                                int base, bool uppercase);
int s21_sprintf_format_float(current_format *cf, va_list args, char *buffer);
int s21_sprintf_format_float_sci(double val, int prec, char *buffer,
                                 bool uppercase);
int s21_sprintf_format_pointer(va_list args, char *buffer);
void s21_sprintf_write_padding(char **dest, int pad, char pad_char,
                               int *written);

#endif  // S21_SPRINTF_H
