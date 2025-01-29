#ifndef S21_SSCANF_H
#define S21_SSCANF_H

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#include "s21_string.h"

typedef enum MODIFIER {
  ERR,
  CHAR,
  DECIMAL,
  INTEGER,
  FLOAT,
  OCTAL,
  STRING,
  UDECIMAL,
  HEX,
  POINTER,
  NUMBER,
  PERCENT
} MODIFIER;
typedef enum LENGTH { NONE, SHORT, LONG, EXTENDED_DOUBLE } LENGTH;
typedef struct current_format {
  bool is_symbol;        // if *char* in format without % then it is read as is,
  char matching_symbol;  // if *char* and the read one differ scan fails

  bool ignore_whitespaces;
  bool ignore;
  s21_size_t width;
  LENGTH length_modifier;
  MODIFIER type_modifier;
} current_format;

current_format* init_format();
void fill_format_default(current_format* settings);
void read_format(const char** format, current_format* settings);
LENGTH get_length(const char ch);
MODIFIER get_modifier(const char ch);
s21_size_t get_width(const char** format);  // is 0 == any?
int s21_sscanf([[maybe_unused]] const char* str,
               [[maybe_unused]] const char* format, ...);

#endif