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
      format++;  // пропускаем '%'
      if (*format == '%') {
        *dest++ = '%';
        written++;
        format++;
        continue;
      }
      print_format *cf = s21_sprintf_init_format();
      s21_sprintf_fill_format_default(cf);
      s21_sprintf_read_format(&format, cf, args);
      s21_sprintf_normalize_format(cf);
      s21_sprintf_apply_format(&dest, cf, args, &written);
      free(cf);
    }
  }
  *dest = '\0';
  va_end(args);
  return written;
}

// Выделение памяти для print_format
print_format *s21_sprintf_init_format() {
  print_format *cf = (print_format *)malloc(sizeof(print_format));
  if (cf) s21_sprintf_fill_format_default(cf);
  return cf;
}

// Инициализация значений по умолчанию
void s21_sprintf_fill_format_default(print_format *cf) {
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

// Разбор спецификатора формата
void s21_sprintf_read_format(const char **format, print_format *cf,
                             va_list args) {
  s21_sprintf_parse_flags(format, cf);
  cf->width = s21_sprintf_get_width(format, args);
  if (**format == '.') {
    (*format)++;
    cf->precision_specified = true;
    cf->precision = s21_sprintf_get_precision(format, args);
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
  if (**format) (*format)++;
}

// Вспомогательная функция для разбора флагов
void s21_sprintf_parse_flags(const char **format, print_format *cf) {
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
s21_size_t s21_sprintf_get_width(const char **format, va_list args) {
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
s21_size_t s21_sprintf_get_precision(const char **format, va_list args) {
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

// Приведение формата к корректному виду для спецификатора '%'
void s21_sprintf_normalize_format(print_format *cf) {
  if (cf->type_modifier == PERCENT) {
    cf->is_symbol = true;
    cf->matching_symbol = '%';
  }
}

// Вспомогательная функция для разворота строки
void s21_sprintf_reverse_str(char *str, int len) {
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
int s21_sprintf_itoa_custom(long long num, char *buf, int base,
                            bool uppercase) {
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
      int rem = num % base;  // Остаток
      buf[i++] =
          (rem < 10) ? ('0' + rem) : ((uppercase ? 'A' : 'a') + rem - 10);
      num /= base;
    }
    if (neg) buf[i++] = '-';
  }
  s21_sprintf_reverse_str(buf, i);
  buf[i] = '\0';
  return i;
}

// Преобразование беззнакового целого в строку
int s21_sprintf_utoa_custom(unsigned long long num, char *buf, int base,
                            bool uppercase) {
  int i = 0;
  if (num == 0) {
    buf[i++] = '0';
  } else {
    while (num) {
      int rem = num % base;  // Остаток
      buf[i++] =
          (rem < 10) ? ('0' + rem) : ((uppercase ? 'A' : 'a') + rem - 10);
      num /= base;
    }
  }
  s21_sprintf_reverse_str(buf, i);
  buf[i] = '\0';
  return i;
}

// Функция форматирования: выбирает нужный обработчик, вычисляет padding и
// записывает результат
void s21_sprintf_apply_format(char **dest, print_format *cf, va_list args,
                              int *written) {
  char temp[1024] = {0};
  int len = 0;
  switch (cf->type_modifier) {
    case CHAR:
      len = s21_sprintf_format_char(args, temp);
      break;
    case STRING:
      len = s21_sprintf_format_string(cf, args, temp);
      break;
    case DECIMAL:
    case INTEGER:
      len = s21_sprintf_format_decimal(cf, args, temp);
      break;
    case UDECIMAL:
      len = s21_sprintf_format_unsigned(cf, args, temp, 10, false);
      break;
    case UOCTAL:
      len = s21_sprintf_format_unsigned(cf, args, temp, 8, false);
      break;
    case UHEX:
      bool up = (cf->spec == 'X');
      len = s21_sprintf_format_unsigned(cf, args, temp, 16, up);
      break;
    case POINTER:
      len = s21_sprintf_format_pointer(args, temp);
      break;
    case FLOAT:
      len = s21_sprintf_format_float(cf, args, temp);
      break;
    case NUMBER:
      int *n_ptr = va_arg(args, int *);
      if (n_ptr) *n_ptr = *written;
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
    s21_sprintf_write_padding(
        dest, pad, (cf->zero_pad && !cf->precision_specified) ? '0' : ' ',
        written);
  s21_memcpy(*dest, temp, len);
  *dest += len;
  *written += len;
  if (cf->left_align) s21_sprintf_write_padding(dest, pad, ' ', written);
}

// Форматирование символа (%c)
int s21_sprintf_format_char(va_list args, char *buffer) {
  char ch = (char)va_arg(args, int);
  buffer[0] = ch;
  buffer[1] = '\0';
  return 1;
}

// Форматирование строки (%s)
int s21_sprintf_format_string(print_format *cf, va_list args, char *buffer) {
  char *s = va_arg(args, char *);
  if (!s) s = "(null)";
  int i = 0;
  while (s[i] && (!cf->precision_specified || i < (int)cf->precision)) {
    buffer[i] = s[i];
    i++;
  }
  buffer[i] = '\0';
  return i;
}

// Форматирование знакового целого (%d/%i)
int s21_sprintf_format_decimal(print_format *cf, va_list args, char *buffer) {
  int is_negative = 0;
  long long num = va_arg(args, int);
  if (num < 0) {
    is_negative = 1;
    num = -num;
  }
  char num_buf[128] = {0};
  int num_len = s21_sprintf_itoa_custom(num, num_buf, 10, false);
  int prec_num_of_zeros = 0;
  if (cf->precision_specified && cf->precision > (s21_size_t)num_len)
    prec_num_of_zeros = cf->precision - num_len;
  int pos = 0;
  if (is_negative)
    buffer[pos++] = '-';
  else if (cf->force_sign)
    buffer[pos++] = '+';
  else if (cf->space)
    buffer[pos++] = ' ';
  for (int i = 0; i < prec_num_of_zeros; i++) {
    buffer[pos++] = '0';
  }
  for (int i = 0; i < num_len; i++) {
    buffer[pos++] = num_buf[i];
  }
  buffer[pos] = '\0';
  return pos;
}

// Форматирование беззнакового целого (%u, %o, %x/%X)
int s21_sprintf_format_unsigned(print_format *cf, va_list args, char *buffer,
                                int base, bool uppercase) {
  unsigned long long num;
  if (cf->length_modifier == LONG)
    num = va_arg(args, unsigned long);
  else if (cf->length_modifier == SHORT)
    num = (unsigned short)va_arg(args, unsigned int);
  else
    num = va_arg(args, unsigned int);
  int len = s21_sprintf_utoa_custom(num, buffer, base, uppercase);
  // Обработка альтернативной формы для восьмеричных и шестнадцатеричных чисел
  if (cf->alt_form) {
    if (base == 8 && buffer[0] != '0') {
      for (int i = len; i >= 0; i--) buffer[i + 1] = buffer[i];
      buffer[0] = '0';
      len++;
    } else if (base == 16 && num != 0) {
      for (int i = len; i >= 0; i--) buffer[i + 2] = buffer[i];
      buffer[0] = '0';
      buffer[1] = (uppercase ? 'X' : 'x');
      len += 2;
    }
  }
  return len;
}

// Вспомогательная функция для форматирования знака
// Возвращает количество записанных символов (0 или 1)
int s21_sprintf_format_sign(print_format *cf, long double val, char *buffer) {
  int pos = 0;
  if (val < 0)
    buffer[pos++] = '-';
  else if (cf->force_sign)
    buffer[pos++] = '+';
  else if (cf->space)
    buffer[pos++] = ' ';
  return pos;
}

// Вспомогательная функция для форматирования дробной части
// Умножает дробь на 10^prec, округляет результат и форматирует его с ведущими
// нулями
int s21_sprintf_format_fractional_part(long double frac, int prec,
                                       char *buffer) {
  int pos = 0;
  long double mult = powl(10, prec);
  long long frac_int = (long long)roundl(frac * mult);
  char frac_buf[64] = {0};
  int frac_len = s21_sprintf_utoa_custom(frac_int, frac_buf, 10, false);
  // Добавляем ведущие нули, если цифр меньше 'prec'
  while (frac_len < prec) {
    buffer[pos++] = '0';
    prec--;
  }
  s21_memcpy(buffer + pos, frac_buf, frac_len);
  pos += frac_len;
  return pos;
}

// Форматирование чисел с плавающей точкой - стандартное представление (%f)
int s21_sprintf_format_float(print_format *cf, va_list args, char *buffer) {
  long double val = (cf->length_modifier == EXTENDED_DOUBLE)
                        ? va_arg(args, long double)
                        : (long double)va_arg(args, double);
  int prec = cf->precision_specified ? cf->precision : 6;
  char orig_spec = cf->spec;
  // Если экспоненциальный формат, сразу форматируем в научной нотации
  if (orig_spec == 'e' || orig_spec == 'E')
    return s21_sprintf_format_float_sci(val, prec, buffer, (orig_spec == 'E'));
  // Для общего формата %g/%G выбираем между фиксированным и экспоненциальным
  if (orig_spec == 'g' || orig_spec == 'G') {
    long double abs_val = fabsl(val);
    int exponent = (abs_val > 0) ? (int)floorl(log10l(abs_val)) : 0;
    if (exponent < -4 || exponent >= prec)
      return s21_sprintf_format_float_sci(val, prec - 1, buffer,
                                          (orig_spec == 'G'));
    else
      cf->spec = 'f';
  }
  int pos = 0;
  long double abs_val = fabsl(val);
  long double mult = powl(10, prec);
  long double rounded = floorl(abs_val * mult + 0.5L) / mult;
  // Если cf->precision_specified && cf->precision==1 и дробная часть равна 0,
  // считаем, что prec = 0
  if (cf->precision_specified && cf->precision == 1 &&
      ((long long)roundl((rounded - (long long)rounded) * mult)) == 0)
    prec = 0;
  pos += s21_sprintf_format_sign(cf, val, buffer);
  long long int_part = (long long)rounded;
  pos += s21_sprintf_itoa_custom(int_part, buffer + pos, 10, false);
  if (prec > 0 ||
      (cf->alt_form && cf->precision_specified && cf->precision == 0)) {
    buffer[pos++] = '.';
    if (prec > 0) {
      long double frac = rounded - int_part;
      pos += s21_sprintf_format_fractional_part(frac, prec, buffer + pos);
    }
  }
  buffer[pos] = '\0';
  // Если оригинальный спецификатор %g/%G, удаляем незначащие нули и точку
  if (orig_spec == 'g' || orig_spec == 'G') {
    int len = s21_strlen(buffer);
    while (len > 0 && buffer[len - 1] == '0') len--;
    if (len > 0 && buffer[len - 1] == '.') len--;
    buffer[len] = '\0';
    pos = len;
  }
  return pos;
}

// Форматирование чисел с плавающей точкой в научной нотации (%e/%E/%g/%G)
int s21_sprintf_format_float_sci(long double val, int prec, char *buffer,
                                 bool uppercase) {
  int pos = 0;
  if (val < 0) {
    buffer[pos++] = '-';
    val = -val;
  }
  int exponent = 0;
  long double norm = val;
  while (val != 0.0 && norm >= 10.0) {
    norm /= 10.0;
    exponent++;
  }
  while (val != 0.0 && norm < 1.0) {
    norm *= 10.0;
    exponent--;
  }
  long double mult = powl(10, prec);
  // Округленное нормальное значение
  long double rnorm = floorl(norm * mult + 0.5L) / mult;
  pos += s21_sprintf_itoa_custom((long long)rnorm, buffer + pos, 10, false);
  buffer[pos++] = '.';
  long long frac_int = (long long)roundl((rnorm - (long long)rnorm) * mult);
  char frac_buf[64] = {0};
  int frac_len = s21_sprintf_utoa_custom(frac_int, frac_buf, 10, false);
  while (frac_len < prec) {
    buffer[pos++] = '0';
    prec--;
  }
  s21_memcpy(buffer + pos, frac_buf, frac_len);
  pos += frac_len;
  // Формируем экспоненту
  buffer[pos++] = (uppercase ? 'E' : 'e');
  buffer[pos++] = (exponent < 0) ? '-' : '+';
  if (exponent < 0) exponent = -exponent;
  if (exponent < 10) buffer[pos++] = '0';
  pos += s21_sprintf_itoa_custom(exponent, buffer + pos, 10, false);
  buffer[pos] = '\0';
  return pos;
}

// Форматирование указателя (%p)
int s21_sprintf_format_pointer(va_list args, char *buffer) {
  void *ptr = va_arg(args, void *);
  unsigned long long addr = (unsigned long long)ptr;
  int pos = 0;
  buffer[pos++] = '0';
  buffer[pos++] = 'x';
  pos += s21_sprintf_utoa_custom(addr, buffer + pos, 16, false);
  return pos;
}

// Запись отступов (padding)
void s21_sprintf_write_padding(char **dest, int pad, char pad_char,
                               int *written) {
  for (int i = 0; i < pad; i++) {
    **dest = pad_char;
    (*dest)++;
    (*written)++;
  }
}