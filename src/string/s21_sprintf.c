#include "s21_sprintf.h"

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

// Главная функция s21_sprintf
int s21_sprintf(char *str, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int written = 0;
  char *dest = str;
  while (*format) {
    if (*format != '%') {
      *dest++ = *format++;
      written++;
    } else {
      format++; // пропускаем '%'
      if (*format == '%') {
        *dest++ = '%';
        written++;
        format++;
        continue;
      }
      current_format *cf = init_format();
      fill_format_default(cf);
      read_format(&format, cf, args);
      normalize_format(cf);
      apply_format(&dest, cf, args, &written);
      free(cf);
    }
  }
  *dest = '\0';
  va_end(args);
  return written;
}

// Выделение памяти для current_format
current_format *init_format() {
  current_format *cf = (current_format *)malloc(sizeof(current_format));
  if (cf)
    fill_format_default(cf);
  return cf;
}

// Инициализация значений по умолчанию
void fill_format_default(current_format *cf) {
  cf->is_symbol = false;
  cf->matching_symbol = '\0';
  cf->left_align = false;
  cf->force_sign = false;
  cf->space = false;
  cf->alt_form = false;
  cf->zero_pad = false;
  cf->width = 0;
  cf->precision_specified = false;
  cf->precision = 0;
  cf->length_modifier = NONE;
  cf->type_modifier = ERR;
  cf->spec = '\0';
}

// Вспомогательная функция для разбора флагов
void parse_flags(const char **format, current_format *cf) {
  while (**format) {
    switch (**format) {
    case '-':
      cf->left_align = true;
      break;
    case '+':
      cf->force_sign = true;
      break;
    case ' ':
      cf->space = true;
      break;
    case '#':
      cf->alt_form = true;
      break;
    case '0':
      cf->zero_pad = true;
      break;
    default:
      return;
    }
    (*format)++;
  }
}

// Получение ширины поля
s21_size_t get_width(const char **format, va_list args) {
  s21_size_t width = 0;
  if (**format == '*') {
    width = (s21_size_t)va_arg(args, int);
    (*format)++;
  } else {
    while (isdigit(**format)) {
      width = width * 10 + (**format - '0');
      (*format)++;
    }
  }
  return width;
}

// Получение точности
s21_size_t get_precision(const char **format, va_list args) {
  s21_size_t precision = 0;
  if (**format == '*') {
    precision = (s21_size_t)va_arg(args, int);
    (*format)++;
  } else {
    while (isdigit(**format)) {
      precision = precision * 10 + (**format - '0');
      (*format)++;
    }
  }
  return precision;
}

// Разбор спецификатора формата
void read_format(const char **format, current_format *cf, va_list args) {
  parse_flags(format, cf);
  cf->width = get_width(format, args);
  if (**format == '.') {
    (*format)++;
    cf->precision_specified = true;
    cf->precision = get_precision(format, args);
  }
  if (**format == 'h' || **format == 'l' || **format == 'L') {
    cf->length_modifier =
        (**format == 'h' ? SHORT : (**format == 'l' ? LONG : EXTENDED_DOUBLE));
    (*format)++;
  }
  cf->spec = **format;
  switch (cf->spec) {
  case 'c':
    cf->type_modifier = CHAR;
    break;
  case 's':
    cf->type_modifier = STRING;
    break;
  case 'd':
  case 'i':
    cf->type_modifier = DECIMAL;
    break;
  case 'u':
    cf->type_modifier = UDECIMAL;
    break;
  case 'o':
    cf->type_modifier = UOCTAL;
    break;
  case 'x':
  case 'X':
    cf->type_modifier = UHEX;
    break;
  case 'p':
    cf->type_modifier = POINTER;
    break;
  case 'n':
    cf->type_modifier = NUMBER;
    break;
  case 'f':
  case 'e':
  case 'E':
  case 'g':
  case 'G':
    cf->type_modifier = FLOAT;
    break;
  default:
    cf->type_modifier = ERR;
    break;
  }
  if (**format)
    (*format)++;
}

// Приведение формата к корректному виду для спецификатора '%'
void normalize_format(current_format *cf) {
  if (cf->type_modifier == PERCENT) {
    cf->is_symbol = true;
    cf->matching_symbol = '%';
  }
}

// Запись отступов (padding)
void write_padding(char **dest, int pad, char pad_char, int *written) {
  for (int i = 0; i < pad; i++) {
    **dest = pad_char;
    (*dest)++;
    (*written)++;
  }
}

// Вспомогательная функция для разворота строки
void reverse_str(char *str, int len) {
  int i = 0, j = len - 1;
  while (i < j) {
    char tmp = str[i];
    str[i] = str[j];
    str[j] = tmp;
    i++;
    j--;
  }
}

// Преобразование знакового целого в строку
int itoa_custom(long long num, char *buf, int base, bool uppercase) {
  int i = 0;
  bool neg = false;
  if (num < 0 && base == 10) {
    neg = true;
    num = -num;
  }
  if (num == 0) {
    buf[i++] = '0';
  } else {
    while (num) {
      int rem = num % base;
      buf[i++] =
          (rem < 10) ? ('0' + rem) : ((uppercase ? 'A' : 'a') + rem - 10);
      num /= base;
    }
    if (neg)
      buf[i++] = '-';
  }
  reverse_str(buf, i);
  buf[i] = '\0';
  return i;
}

// Преобразование беззнакового целого в строку
int utoa_custom(unsigned long long num, char *buf, int base, bool uppercase) {
  int i = 0;
  if (num == 0) {
    buf[i++] = '0';
  } else {
    while (num) {
      int rem = num % base;
      buf[i++] =
          (rem < 10) ? ('0' + rem) : ((uppercase ? 'A' : 'a') + rem - 10);
      num /= base;
    }
  }
  reverse_str(buf, i);
  buf[i] = '\0';
  return i;
}

// Форматирование символа (%c)
int format_char(va_list args, char *buffer) {
  char ch = (char)va_arg(args, int);
  buffer[0] = ch;
  buffer[1] = '\0';
  return 1;
}

// Форматирование строки (%s)
int format_string(current_format *cf, va_list args, char *buffer) {
  char *s = va_arg(args, char *);
  if (!s)
    s = "(null)";
  int i = 0;
  while (s[i] && (!cf->precision_specified || i < (int)cf->precision)) {
    buffer[i] = s[i];
    i++;
  }
  buffer[i] = '\0';
  return i;
}

// Форматирование знакового целого (%d/%i)
int format_decimal(current_format *cf, va_list args, char *buffer) {
  long long num;
  if (cf->length_modifier == LONG)
    num = va_arg(args, long);
  else if (cf->length_modifier == SHORT)
    num = (short)va_arg(args, int);
  else
    num = va_arg(args, int);
  return itoa_custom(num, buffer, 10, false);
}

// Форматирование беззнакового целого (%u, %o, %x/%X)
int format_unsigned(current_format *cf, va_list args, char *buffer, int base,
                    bool uppercase) {
  unsigned long long num;
  if (cf->length_modifier == LONG)
    num = va_arg(args, unsigned long);
  else if (cf->length_modifier == SHORT)
    num = (unsigned short)va_arg(args, unsigned int);
  else
    num = va_arg(args, unsigned int);
  return utoa_custom(num, buffer, base, uppercase);
}

// Форматирование чисел с плавающей точкой - стандартное представление (%f)
int format_float(current_format *cf, va_list args, char *buffer) {
  double val = va_arg(args, double);
  int prec = cf->precision_specified ? cf->precision : 6;
  if (cf->spec == 'f') {
    double abs_val = (val < 0) ? -val : val;
    long long int_part = (long long)abs_val;
    double frac = abs_val - int_part;
    int pos = itoa_custom(int_part, buffer, 10, false);
    buffer[pos++] = '.';
    for (int i = 0; i < prec; i++) {
      frac *= 10;
      int d = (int)frac;
      buffer[pos++] = '0' + d;
      frac -= d;
    }
    buffer[pos] = '\0';
    return pos;
  } else if (cf->spec == 'e' || cf->spec == 'E' || cf->spec == 'g' ||
             cf->spec == 'G') {
    return format_float_sci(val, prec, buffer,
                            (cf->spec == 'E' || cf->spec == 'G'));
  }
  return 0;
}

// Форматирование чисел с плавающей точкой в научной нотации (%e/%E/%g/%G)
int format_float_sci(double val, int prec, char *buffer, bool uppercase) {
  int pos = 0;
  if (val < 0) {
    buffer[pos++] = '-';
    val = -val;
  }
  int exponent = 0;
  double norm = val;
  if (val != 0.0) {
    while (norm >= 10.0) {
      norm /= 10.0;
      exponent++;
    }
    while (norm < 1.0) {
      norm *= 10.0;
      exponent--;
    }
  }
  pos += itoa_custom((long long)norm, buffer + pos, 10, false);
  buffer[pos++] = '.';
  double frac = norm - (long long)norm;
  for (int i = 0; i < prec; i++) {
    frac *= 10;
    int d = (int)frac;
    buffer[pos++] = '0' + d;
    frac -= d;
  }
  buffer[pos++] = (uppercase ? 'E' : 'e');
  if (exponent < 0) {
    buffer[pos++] = '-';
    exponent = -exponent;
  } else {
    buffer[pos++] = '+';
  }
  if (exponent < 10) {
    buffer[pos++] = '0';
  }
  pos += itoa_custom(exponent, buffer + pos, 10, false);
  buffer[pos] = '\0';
  return pos;
}

// Форматирование указателя (%p)
int format_pointer(va_list args, char *buffer) {
  void *ptr = va_arg(args, void *);
  unsigned long long addr = (unsigned long long)ptr;
  int pos = 0;
  buffer[pos++] = '0';
  buffer[pos++] = 'x';
  pos += utoa_custom(addr, buffer + pos, 16, false);
  return pos;
}

// Функция форматирования: выбирает нужный обработчик, вычисляет padding и
// записывает результат
void apply_format(char **dest, current_format *cf, va_list args, int *written) {
  char temp[1024] = {0};
  int len = 0;
  switch (cf->type_modifier) {
  case CHAR:
    len = format_char(args, temp);
    break;
  case STRING:
    len = format_string(cf, args, temp);
    break;
  case DECIMAL:
  case INTEGER:
    len = format_decimal(cf, args, temp);
    break;
  case UDECIMAL:
    len = format_unsigned(cf, args, temp, 10, false);
    break;
  case UOCTAL:
    len = format_unsigned(cf, args, temp, 8, false);
    break;
  case UHEX:
    bool up = (cf->spec == 'X');
    len = format_unsigned(cf, args, temp, 16, up);
    break;
  case POINTER:
    len = format_pointer(args, temp);
    break;
  case FLOAT:
    len = format_float(cf, args, temp);
    break;
  case NUMBER:
    int *n_ptr = va_arg(args, int *);
    if (n_ptr)
      *n_ptr = *written;
    return;
  case PERCENT:
    temp[0] = '%';
    temp[1] = '\0';
    len = 1;
    break;
  default:
    temp[0] = cf->spec;
    temp[1] = '\0';
    len = 1;
    break;
  }
  int pad = (cf->width > (s21_size_t)len) ? cf->width - len : 0;
  if (!cf->left_align)
    write_padding(dest, pad,
                  (cf->zero_pad && !cf->precision_specified) ? '0' : ' ',
                  written);
  s21_memcpy(*dest, temp, len);
  *dest += len;
  *written += len;
  if (cf->left_align)
    write_padding(dest, pad, ' ', written);
}
