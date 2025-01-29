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
    mod = OCTAL;
  else if (ch == 's')
    mod = STRING;
  else if (ch == 'u')
    mod = UDECIMAL;
  else if (ch == 'x' || ch == 'X')
    mod = HEX;
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

int s21_sscanf([[maybe_unused]] const char* str, const char* format, ...) {
  int count = 0;
  va_list args;
  va_start(args, format);
  current_format* settings = init_format();
  while (s21_strlen(format) > 0) {
    fill_format_default(settings);
    read_format(&format, settings);
    // get input
    ;
  }
  free(settings);
  va_end(args);
  if (count == 0) count = EOF;  // needed by specs if no input read
  return count;
}