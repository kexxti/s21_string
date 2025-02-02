#include "s21_sscanf.h"

int s21_sscanf(const char* str, const char* format, ...) {
  int read_bytes_count = 0;
  int successful_scans_count = 0;
  bool should_return_EOF = false;
  va_list args;
  va_start(args, format);
  bool is_scan_failed = false;
  current_format* settings = init_format();
  while (!is_scan_failed && s21_strlen(format) > 0) {
    fill_format_default(settings);
    read_format(&format, settings);
    normalize_format(settings);
    void* dest = NULL;
    void** pointer_dest = NULL;
    if (needs_dest(settings)) {
      if (settings->type_modifier == POINTER)
        pointer_dest = va_arg(args, void**);
      else
        dest = get_next_dest(settings, args);
    }
    const char* temp = str;  // to track EOF
    is_scan_failed =
        !get_input(&str, settings, dest, pointer_dest, &read_bytes_count);
    if (!is_scan_failed && !settings->is_symbol && !settings->ignore &&
        settings->type_modifier != NUMBER)
      successful_scans_count++;
    if (temp == str && s21_strlen(temp) == 0) should_return_EOF = true;
  }
  free(settings);
  va_end(args);
  if (successful_scans_count == 0 && should_return_EOF)
    successful_scans_count = EOF;
  return successful_scans_count;
}

current_format* init_format() {
  current_format* ptr = malloc(sizeof(current_format));
  if (ptr) fill_format_default(ptr);
  return ptr;
}

void fill_format_default(current_format* settings) {
  settings->is_symbol = false;
  settings->matching_symbol = '\0';
  settings->ignore_whitespaces = false;
  settings->ignore = false;
  settings->width = 0;
  settings->length_modifier = NONE;
  settings->type_modifier = ERR;
}

void read_format(const char** format, current_format* settings) {
  int ch;
  while (isspace((ch = **format))) {
    settings->ignore_whitespaces = true;
    (*format)++;
  }
  if (ch != '%') {
    settings->is_symbol = true;
    settings->matching_symbol = ch;
    (*format)++;
  } else {
    (*format)++;
    if (**format == '*') {
      settings->ignore = true;
      (*format)++;
    }
    settings->width = get_width(format);
    settings->length_modifier = get_length(**format);
    if (settings->length_modifier) (*format)++;
    settings->type_modifier = get_modifier(**format);  // what if ERR ? !!!
    if (settings->type_modifier) (*format)++;
  }
}

LENGTH get_length(const char ch) {
  LENGTH mod = NONE;
  if (ch == 'h')
    mod = SHORT;
  else if (ch == 'l')
    mod = LONG;
  else if (ch == 'L')
    mod = EXTENDED_DOUBLE;
  return mod;
}

MODIFIER get_modifier(const char ch) {
  MODIFIER mod = ERR;
  if (ch == 'c')
    mod = CHAR;
  else if (ch == 'd')
    mod = DECIMAL;
  else if (ch == 'i')
    mod = INTEGER;
  else if (ch == 'e' || ch == 'E' || ch == 'f' || ch == 'g' || ch == 'G')
    mod = FLOAT;
  else if (ch == 'o')
    mod = UOCTAL;
  else if (ch == 's')
    mod = STRING;
  else if (ch == 'u')
    mod = UDECIMAL;
  else if (ch == 'x' || ch == 'X')
    mod = UHEX;
  else if (ch == 'p')
    mod = POINTER;
  else if (ch == 'n')
    mod = NUMBER;
  else if (ch == '%')
    mod = PERCENT;
  return mod;
}

s21_size_t get_width(const char** format) {
  s21_size_t width = 0;
  while (isdigit(**format)) {
    width = width * 10 + (**format - '0');
    (*format)++;
  }
  return width;
}

void normalize_format(current_format* settings) {
  // add check to width > 0 and return whether format is possible?
  if (settings->type_modifier == PERCENT) {
    settings->is_symbol = true;
    settings->matching_symbol = '%';
  }
  if (!settings->is_symbol && settings->type_modifier != CHAR &&
      settings->type_modifier != NUMBER)
    settings->ignore_whitespaces = true;
}

bool needs_dest(current_format* settings) {
  bool result = true;
  if (settings->ignore || settings->is_symbol ||
      settings->type_modifier == PERCENT)
    result = false;
  return result;
}

void* get_next_dest(current_format* settings, va_list args) {
  void* dest = NULL;
  switch (settings->type_modifier) {
    case CHAR:
    case STRING:
      if (settings->length_modifier == LONG)
        dest = va_arg(args, wchar_t*);
      else
        dest = va_arg(args, char*);
      break;
    case INTEGER:
    case DECIMAL:
    case NUMBER:
      if (settings->length_modifier == NONE)
        dest = va_arg(args, int*);
      else if (settings->length_modifier == SHORT)
        dest = va_arg(args, short int*);
      else if (settings->length_modifier == LONG)
        dest = va_arg(args, long int*);
      break;
    case UOCTAL:
    case UHEX:
    case UDECIMAL:
      if (settings->length_modifier == NONE)
        dest = va_arg(args, unsigned int*);
      else if (settings->length_modifier == SHORT)
        dest = va_arg(args, unsigned short int*);
      else if (settings->length_modifier == LONG)
        dest = va_arg(args, unsigned long int*);
      break;
    case FLOAT:
      if (settings->length_modifier == NONE)
        dest = va_arg(args, float*);
      else if (settings->length_modifier == LONG)
        dest = va_arg(args, double*);
      else if (settings->length_modifier == EXTENDED_DOUBLE)
        dest = va_arg(args, long double*);
      break;
    default:
      break;
  }
  return dest;
}

bool get_symbol(const char** str, current_format* settings,
                int* read_bytes_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) skip_whitespaces(str, read_bytes_count);
  if ((*str)[0] == settings->matching_symbol) {
    is_scan_succeded = true;
    (*str)++;
    (*read_bytes_count)++;
  }
  return is_scan_succeded;
}

bool get_char(const char** str, current_format* settings, void* dest,
              int* read_bytes_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) skip_whitespaces(str, read_bytes_count);
  if (settings->length_modifier == NONE) {
    char ch = **str;
    if (ch) {
      is_scan_succeded = true;
      (*str)++;
      (*read_bytes_count)++;
      if (!settings->ignore) *((char*)dest) = ch;
    }
  } else if (settings->length_modifier == LONG) {
    wchar_t ch = **str;
    if (ch) {
      is_scan_succeded = true;
      (*str)++;
      (*read_bytes_count)++;
      if (!settings->ignore) *((wchar_t*)dest) = ch;
    }
  }
  return is_scan_succeded;
}

bool is_negative_ato(const char** str, int* read_bytes_count) {
  bool negative = false;
  if (**str == '-') {
    negative = true;
    (*str)++;
    if (read_bytes_count) (*read_bytes_count)++;
  }
  return negative;
}

void atoi_move_to_next(s21_size_t* length, const char** str,
                       int* read_bytes_count, bool* is_scan_succeded) {
  (*length)++;
  (*str)++;
  if (read_bytes_count) (*read_bytes_count)++;
  if (is_scan_succeded && !*is_scan_succeded) *is_scan_succeded = true;
}
int s21_atoi(const char** str, current_format* settings, int* read_bytes_count,
             bool* is_scan_succeded) {
  int result = 0;
  s21_size_t length = 0;
  bool negative = is_negative_ato(str, read_bytes_count);
  if (negative) length++;
  while (isdigit(**str) && (length < settings->width || !settings->width)) {
    result = result * 10 + (**str - '0');
    atoi_move_to_next(&length, str, read_bytes_count, is_scan_succeded);
  }
  if (negative) result *= -1;
  return result;
}

short int s21_atosi(const char** str, current_format* settings,
                    int* read_bytes_count, bool* is_scan_succeded) {
  short int result = 0;
  s21_size_t length = 0;
  bool negative = is_negative_ato(str, read_bytes_count);
  if (negative) length++;
  while (isdigit(**str) && (length < settings->width || !settings->width)) {
    result = result * 10 + (**str - '0');
    atoi_move_to_next(&length, str, read_bytes_count, is_scan_succeded);
  }
  if (negative) result *= -1;
  return result;
}

long int s21_atol(const char** str, current_format* settings,
                  int* read_bytes_count, bool* is_scan_succeded) {
  long int result = 0;
  s21_size_t length = 0;
  bool negative = is_negative_ato(str, read_bytes_count);
  if (negative) length++;
  while (isdigit(**str) && (length < settings->width || !settings->width)) {
    result = result * 10 + (**str - '0');
    atoi_move_to_next(&length, str, read_bytes_count, is_scan_succeded);
  }
  if (negative) result *= -1;
  return result;
}

bool get_decimal(const char** str, current_format* settings, void* dest,
                 int* read_bytes_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) skip_whitespaces(str, read_bytes_count);
  if (settings->length_modifier == NONE) {
    int value = s21_atoi(str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore) *(int*)dest = value;
  } else if (settings->length_modifier == SHORT) {
    short int value =
        s21_atosi(str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore) *(short int*)dest = value;
  } else if (settings->length_modifier == LONG) {
    long int value =
        s21_atol(str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore) *(long int*)dest = value;
  }
  return is_scan_succeded;
}

void skip_whitespaces(const char** str, int* read_bytes_count) {
  while (isspace(**str)) {
    (*str)++;
    (*read_bytes_count)++;
  }
}

bool get_udecimal(const char** str, current_format* settings, void* dest,
                  int* read_bytes_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) skip_whitespaces(str, read_bytes_count);
  if (settings->length_modifier == NONE) {  // seriosly lol, only cast
    unsigned int value = (unsigned int)s21_atoi(str, settings, read_bytes_count,
                                                &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore) *(unsigned int*)dest = value;
  } else if (settings->length_modifier == SHORT) {
    unsigned short int value = (unsigned short int)s21_atosi(
        str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore)
      *(unsigned short int*)dest = value;
  } else if (settings->length_modifier == LONG) {
    unsigned long int value = (unsigned long int)s21_atol(
        str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore)
      *(unsigned long int*)dest = value;
  }
  return is_scan_succeded;
}

int s21_a_to_octali(const char** str, current_format* settings,
                    int* read_bytes_count, bool* is_scan_succeded) {
  int result = 0;
  s21_size_t length = 0;
  bool negative = is_negative_ato(str, read_bytes_count);
  if (negative) length++;
  while (**str < '8' && **str >= '0' &&
         (length < settings->width || !settings->width)) {
    result = result * 8 + (**str - '0');
    atoi_move_to_next(&length, str, read_bytes_count, is_scan_succeded);
  }
  if (negative) result *= -1;
  return result;
}
short int s21_a_to_octalsi(const char** str, current_format* settings,
                           int* read_bytes_count, bool* is_scan_succeded) {
  short int result = 0;
  s21_size_t length = 0;
  bool negative = is_negative_ato(str, read_bytes_count);
  if (negative) length++;
  while (**str < '8' && **str >= '0' &&
         (length < settings->width || !settings->width)) {
    result = result * 8 + (**str - '0');
    atoi_move_to_next(&length, str, read_bytes_count, is_scan_succeded);
  }
  if (negative) result *= -1;
  return result;
}
long int s21_a_to_octall(const char** str, current_format* settings,
                         int* read_bytes_count, bool* is_scan_succeded) {
  long int result = 0;
  s21_size_t length = 0;
  bool negative = is_negative_ato(str, read_bytes_count);
  if (negative) length++;
  while (**str < '8' && **str >= '0' &&
         (length < settings->width || !settings->width)) {
    result = result * 8 + (**str - '0');
    atoi_move_to_next(&length, str, read_bytes_count, is_scan_succeded);
  }
  if (negative) result *= -1;
  return result;
}

bool get_uoctal(const char** str, current_format* settings, void* dest,
                int* read_bytes_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) skip_whitespaces(str, read_bytes_count);
  if (settings->length_modifier == NONE) {
    unsigned int value =
        s21_a_to_octali(str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore) *(unsigned int*)dest = value;
  } else if (settings->length_modifier == SHORT) {
    unsigned short int value =
        s21_a_to_octalsi(str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore)
      *(unsigned short int*)dest = value;
  } else if (settings->length_modifier == LONG) {
    unsigned long int value =
        s21_a_to_octall(str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore)
      *(unsigned long int*)dest = value;
  }
  return is_scan_succeded;
}

void skip_hex_prefix(s21_size_t* length, const char** str,
                     current_format* settings, int* read_bytes_count) {
  if (s21_strlen(*str) >= 2 && (*str)[0] == '0' &&
      ((*str)[1] == 'x' || (*str)[1] == 'X')) {
    s21_size_t delta =
        settings->width >= 2 || !settings->width ? 2 : settings->width;
    (*str) += delta;
    if (read_bytes_count) (*read_bytes_count) += delta;
    (*length) += delta;
  }
}

bool skip_hex_atoi_prefixes(s21_size_t* length, const char** str,
                            current_format* settings, int* read_bytes_count) {
  bool negative = is_negative_ato(str, read_bytes_count);
  if (negative) (*length)++;
  skip_hex_prefix(length, str, settings, read_bytes_count);
  return negative;
}

int s21_a_to_hexi(const char** str, current_format* settings,
                  int* read_bytes_count, bool* is_scan_succeded) {
  int result = 0;
  s21_size_t length = 0;
  bool negative =
      skip_hex_atoi_prefixes(&length, str, settings, read_bytes_count);
  while (((tolower(**str) <= 'f' && tolower(**str) >= 'a') || isdigit(**str)) &&
         (length < settings->width || !settings->width)) {
    result = result * 16;
    if (isdigit(**str))
      result += (**str - '0');
    else
      result += (tolower(**str) - 'a' + 10);
    atoi_move_to_next(&length, str, read_bytes_count, is_scan_succeded);
  }
  if (negative) result *= -1;
  return result;
}
short int s21_a_to_hexsi(const char** str, current_format* settings,
                         int* read_bytes_count, bool* is_scan_succeded) {
  short int result = 0;
  s21_size_t length = 0;
  bool negative =
      skip_hex_atoi_prefixes(&length, str, settings, read_bytes_count);
  while (((tolower(**str) <= 'f' && tolower(**str) >= 'a') || isdigit(**str)) &&
         (length < settings->width || !settings->width)) {
    result = result * 16;
    if (isdigit(**str))
      result += (**str - '0');
    else
      result += (tolower(**str) - 'a' + 10);
    atoi_move_to_next(&length, str, read_bytes_count, is_scan_succeded);
  }
  if (negative) result *= -1;
  return result;
}
long int s21_a_to_hexl(const char** str, current_format* settings,
                       int* read_bytes_count, bool* is_scan_succeded) {
  long int result = 0;
  s21_size_t length = 0;
  bool negative =
      skip_hex_atoi_prefixes(&length, str, settings, read_bytes_count);
  while (((tolower(**str) <= 'f' && tolower(**str) >= 'a') || isdigit(**str)) &&
         (length < settings->width || !settings->width)) {
    result = result * 16;
    if (isdigit(**str))
      result += (**str - '0');
    else
      result += (tolower(**str) - 'a' + 10);
    atoi_move_to_next(&length, str, read_bytes_count, is_scan_succeded);
  }
  if (negative) result *= -1;
  return result;
}

bool get_uhex(const char** str, current_format* settings, void* dest,
              int* read_bytes_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) skip_whitespaces(str, read_bytes_count);

  bool has_prefix = false;
  if (s21_strlen(*str) >= 2 && (*str)[0] == '0' &&
      ((*str)[1] == 'x' || (*str)[1] == 'X'))
    has_prefix = true;

  if (settings->length_modifier == NONE) {
    unsigned int value =
        s21_a_to_hexi(str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore)
      *(unsigned int*)dest = value;
    else if (has_prefix && !settings->ignore)
      *(unsigned int*)dest = 0;
  } else if (settings->length_modifier == SHORT) {
    unsigned short int value =
        s21_a_to_hexsi(str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore)
      *(unsigned short int*)dest = value;
    else if (has_prefix && !settings->ignore)
      *(unsigned short int*)dest = 0;
  } else if (settings->length_modifier == LONG) {
    unsigned long int value =
        s21_a_to_hexl(str, settings, read_bytes_count, &is_scan_succeded);
    if (is_scan_succeded && !settings->ignore)
      *(unsigned long int*)dest = value;
    else if (has_prefix && !settings->ignore)
      *(unsigned long int*)dest = 0;
  }
  if (has_prefix) is_scan_succeded = true;
  return is_scan_succeded;
}

bool get_integer(const char** str, current_format* settings, void* dest,
                 int* read_bytes_count) {
  bool is_scan_succeded = false;

  if (**str == '0') {
    if ((*str)[1] == 'x' || (*str)[1] == 'X')
      is_scan_succeded = get_uhex(str, settings, dest, read_bytes_count);
    else
      is_scan_succeded = get_uoctal(str, settings, dest, read_bytes_count);
  } else
    is_scan_succeded = get_decimal(str, settings, dest, read_bytes_count);

  return is_scan_succeded;
}

bool get_string(const char** str, current_format* settings, void* dest,
                int* read_bytes_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) skip_whitespaces(str, read_bytes_count);
  char* result;
  if (!settings->ignore) result = (char*)dest;
  s21_size_t length = 0;
  while (!isspace(**str) && **str != '\0' &&
         (!settings->width || length < settings->width)) {
    if (!settings->ignore) {
      *result = **str;
      result++;
    }
    (*str)++;
    (*read_bytes_count)++;
    length++;
  }
  if (!settings->ignore && length) *result = '\0';
  if (length) is_scan_succeded = true;
  return is_scan_succeded;
}

bool get_number(const char** str, current_format* settings, void* dest,
                int* read_bytes_count) {
  if (settings->ignore_whitespaces) skip_whitespaces(str, read_bytes_count);
  if (settings->length_modifier == NONE) *(int*)dest = *read_bytes_count;
  if (settings->length_modifier == SHORT) *(short int*)dest = *read_bytes_count;
  if (settings->length_modifier == LONG) *(long int*)dest = *read_bytes_count;
  return true;
}

bool get_pointer(const char** str, current_format* settings, void** dest,
                 int* read_bytes_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) skip_whitespaces(str, read_bytes_count);
  bool has_prefix = false;
  if (**str == '0') has_prefix = true;
  unsigned long int value = (unsigned long int)s21_a_to_hexl(
      str, settings, read_bytes_count, &is_scan_succeded);  // it's always hex
  if (is_scan_succeded && !settings->ignore) {
    *dest = (void*)value;
  } else if (has_prefix && !settings->ignore)
    *dest = (void*)0;
  if (has_prefix) is_scan_succeded = true;
  return is_scan_succeded;
}

bool get_input(const char** str, current_format* settings, void* dest,
               void** pointer_dest, int* read_bytes_count) {
  bool is_scan_succeded = false;
  if (settings->is_symbol)
    is_scan_succeded = get_symbol(str, settings, read_bytes_count);
  else {
    if (settings->type_modifier == CHAR)
      is_scan_succeded = get_char(str, settings, dest, read_bytes_count);
    if (settings->type_modifier == DECIMAL)
      is_scan_succeded = get_decimal(str, settings, dest, read_bytes_count);
    if (settings->type_modifier == INTEGER)
      is_scan_succeded = get_integer(str, settings, dest, read_bytes_count);
    // if (settings->type_modifier == FLOAT)
    //   is_scan_succeded = get_float(str, settings, *dest, read_bytes_count);
    if (settings->type_modifier == UOCTAL)
      is_scan_succeded = get_uoctal(str, settings, dest, read_bytes_count);
    if (settings->type_modifier == STRING)
      is_scan_succeded = get_string(str, settings, dest, read_bytes_count);
    if (settings->type_modifier == UDECIMAL)
      is_scan_succeded = get_udecimal(str, settings, dest, read_bytes_count);
    if (settings->type_modifier == UHEX)
      is_scan_succeded = get_uhex(str, settings, dest, read_bytes_count);
    if (settings->type_modifier == POINTER)
      is_scan_succeded =
          get_pointer(str, settings, pointer_dest, read_bytes_count);
    if (settings->type_modifier == NUMBER)
      is_scan_succeded = get_number(str, settings, dest, read_bytes_count);
  }
  return is_scan_succeded;
}