#include "s21_sscanf.h"

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
}

bool needs_dest(current_format* settings) {
  bool result = true;
  if (settings->is_symbol || settings->type_modifier == PERCENT) result = false;
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
    case POINTER:
      dest = va_arg(args, void*);
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

bool get_symbol(const char** str, current_format* settings, int* read_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) {
    while (isspace(**str)) {
      (*str)++;
      (*read_count)++;
    }
  }
  if ((*str)[0] == settings->matching_symbol) {
    is_scan_succeded = true;
    (*str)++;
    (*read_count)++;
  }
  return is_scan_succeded;
}

bool get_char(const char** str, current_format* settings, void* dest,
              int* read_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) {
    while (isspace(**str)) {
      (*str)++;
      (*read_count)++;
    }
  }
  char ch = (*str)[0];
  if (ch) {
    is_scan_succeded = true;
    (*str)++;
    (*read_count)++;
    *((char*)dest) = ch;
  }
  return is_scan_succeded;
}

bool is_negative_ato(const char** str, int* read_count) {
  bool negative = false;
  if (**str == '-') {
    negative = true;
    (*str)++;
    if (read_count) (*read_count)++;
  }
  return negative;
}
int s21_atoi(const char** str, int* read_count, bool* is_scan_succeded) {
  bool negative = is_negative_ato(str, read_count);
  int result = 0;
  while (isdigit(**str)) {
    result = result * 10 + (**str - '0');
    (*str)++;
    if (read_count) (*read_count)++;
    if (is_scan_succeded && !*is_scan_succeded) *is_scan_succeded = true;
  }
  if (negative) result *= -1;
  return result;
}

short int s21_atosi(const char** str, int* read_count, bool* is_scan_succeded) {
  bool negative = is_negative_ato(str, read_count);
  short int result = 0;
  while (isdigit(**str)) {
    result = result * 10 + (**str - '0');
    (*str)++;
    if (read_count) (*read_count)++;
    if (is_scan_succeded && !*is_scan_succeded) *is_scan_succeded = true;
  }
  if (negative) result *= -1;
  return result;
}

long int s21_atol(const char** str, int* read_count, bool* is_scan_succeded) {
  bool negative = is_negative_ato(str, read_count);
  long int result = 0;
  while (isdigit(**str)) {
    result = result * 10 + (**str - '0');
    (*str)++;
    if (read_count) (*read_count)++;
    if (is_scan_succeded && !*is_scan_succeded) *is_scan_succeded = true;
  }
  if (negative) result *= -1;
  return result;
}

bool get_decimal(const char** str, current_format* settings, void* dest,
                 int* read_count) {
  bool is_scan_succeded = false;
  if (settings->ignore_whitespaces) {
    while (isspace(**str)) {
      (*str)++;
      (*read_count)++;
    }
  }
  if (settings->length_modifier == NONE)
    *(int*)dest = s21_atoi(str, read_count, &is_scan_succeded);
  else if (settings->length_modifier == SHORT)
    *(short int*)dest = s21_atosi(str, read_count, &is_scan_succeded);
  else if (settings->length_modifier == LONG)
    *(long int*)dest = s21_atol(str, read_count, &is_scan_succeded);
  return is_scan_succeded;
}

// bool get_char(const char** str, current_format* settings, void* dest,
//               int* read_count) {
//   bool is_scan_succeded = false;
//   if (settings->ignore_whitespaces) {
//     while (isspace(**str)) {
//       (*str)++;
//       (*read_count)++;
//     }
//   }
//   char ch = (*str)[0];
//   if (ch) {
//     is_scan_succeded = true;
//     (*str)++;
//     (*read_count)++;
//     *((char*)dest) = ch;
//   }
//   return is_scan_succeded;
// }
// //               int* read_count) {
// //   bool is_scan_succeded = false;
// //   if (settings->ignore_whitespaces) {
// //     while (isspace(**str)) {
// //       (*str)++;
// //       (*read_count)++;
// //     }
// //   }
// //   char ch = (*str)[0];
// //   if (ch) {
// //     is_scan_succeded = true;
// //     (*str)++;
// //     (*read_count)++;
// //     *((char*)dest) = ch;
// //   }
// //   return is_scan_succeded;
// // }

// bool get_input(const char** str, current_format* settings, void* dest,
//                int* read_count) {
//   bool is_scan_succeded = false;
//   if (settings->is_symbol)
//     is_scan_succeded = get_symbol(str, settings, read_count);
//   else {
//     if (settings->type_modifier == CHAR)
//       is_scan_succeded = get_char(str, settings, dest, read_count);
//     if (settings->type_modifier == DECIMAL)
//       is_scan_succeded = get_decimal(str, settings, dest, read_count);
//     if (settings->type_modifier == INTEGER)
//       is_scan_succeded = get_integer(str, settings, dest, read_count);
//     if (settings->type_modifier == FLOAT)
//       is_scan_succeded = get_float(str, settings, dest, read_count);
//     if (settings->type_modifier == UOCTAL)
//       is_scan_succeded = get_uoctal(str, settings, dest, read_count);
//     if (settings->type_modifier == STRING)
//       is_scan_succeded = get_string(str, settings, dest, read_count);
//     if (settings->type_modifier == UDECIMAL)
//       is_scan_succeded = get_udecimal(str, settings, dest, read_count);
//     if (settings->type_modifier == UHEX)
//       is_scan_succeded = get_uhex(str, settings, dest, read_count);
//     if (settings->type_modifier == POINTER)
//       is_scan_succeded = get_pointer(str, settings, dest, read_count);
//     if (settings->type_modifier == NUMBER)
//       is_scan_succeded = get_number(str, settings, dest, read_count);
//   }
//   return is_scan_succeded;
// }

// int s21_sscanf(const char* str, const char* format, ...) {
//   int read_count = 0;
//   va_list args;
//   va_start(args, format);
//   bool is_scan_failed = false;
//   current_format* settings = init_format();
//   while (!is_scan_failed && s21_strlen(format) > 0) {
//     fill_format_default(settings);
//     read_format(&format, settings);
//     normalize_format(settings);
//     void* dest = NULL;
//     if (needs_dest(settings)) dest = get_next_dest(settings, args);
//     is_scan_failed = !get_input(&str, settings, dest, &read_count);
//   }
//   free(settings);
//   va_end(args);
//   if (read_count == 0) read_count = EOF;  // by std if no input read
//   return read_count;
// }