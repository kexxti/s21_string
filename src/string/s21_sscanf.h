#ifndef S21_SSCANF_H
#define S21_SSCANF_H

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>  // for EOF

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
  PERCENT  // can't ignore it, behaves as normal symbol
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

int s21_sscanf(const char *str, const char *format, ...);
current_format *init_format();
void fill_format_default(current_format *settings);
void read_format(const char **format, current_format *settings);
LENGTH get_length(const char ch);
MODIFIER get_modifier(const char ch);
s21_size_t get_width(const char **format);
void normalize_format(current_format *settings);
bool needs_dest(current_format *settings);
void *get_next_dest(current_format *settings, va_list args);
bool get_symbol(const char **str, current_format *settings,
                int *read_bytes_count);
bool get_char(const char **str, current_format *settings, void *dest,
              int *read_bytes_count);
bool is_negative_ato(s21_size_t *length, const char **str,
                     int *read_bytes_count);
void atoi_move_to_next(s21_size_t *length, const char **str,
                       int *read_bytes_count, bool *is_scan_succeded);
int s21_atoi(const char **str, current_format *settings, int *read_bytes_count,
             bool *is_scan_succeded);
short int s21_atosi(const char **str, current_format *settings,
                    int *read_bytes_count, bool *is_scan_succeded);
long int s21_atol(const char **str, current_format *settings,
                  int *read_bytes_count, bool *is_scan_succeded);
bool get_decimal(const char **str, current_format *settings, void *dest,
                 int *read_bytes_count);
void skip_whitespaces(const char **str, int *read_bytes_count);
bool get_udecimal(const char **str, current_format *settings, void *dest,
                  int *read_bytes_count);
int s21_a_to_octali(const char **str, current_format *settings,
                    int *read_bytes_count, bool *is_scan_succeded);
short int s21_a_to_octalsi(const char **str, current_format *settings,
                           int *read_bytes_count, bool *is_scan_succeded);
long int s21_a_to_octall(const char **str, current_format *settings,
                         int *read_bytes_count, bool *is_scan_succeded);
bool get_uoctal(const char **str, current_format *settings, void *dest,
                int *read_bytes_count);
void skip_hex_prefix(s21_size_t *length, const char **str,
                     current_format *settings, int *read_bytes_count);
bool skip_hex_atoi_prefixes(s21_size_t *length, const char **str,
                            current_format *settings, int *read_bytes_count);
int s21_a_to_hexi(const char **str, current_format *settings,
                  int *read_bytes_count, bool *is_scan_succeded);
short int s21_a_to_hexsi(const char **str, current_format *settings,
                         int *read_bytes_count, bool *is_scan_succeded);
long int s21_a_to_hexl(const char **str, current_format *settings,
                       int *read_bytes_count, bool *is_scan_succeded);
bool get_uhex(const char **str, current_format *settings, void *dest,
              int *read_bytes_count);
bool get_integer(const char **str, current_format *settings, void *dest,
                 int *read_bytes_count);
bool get_string(const char **str, current_format *settings, void *dest,
                int *read_bytes_count);
bool get_number(const char **str, current_format *settings, void *dest,
                int *read_bytes_count);
bool get_pointer(const char **str, current_format *settings, void **dest,
                 int *read_bytes_count);
bool get_float(const char **str, current_format *settings, void *dest,
               int *read_bytes_count);
long double read_extended_double(const char **str, current_format *settings,
                                 int *read_bytes_count, bool *is_scan_succeded);

void goto_next_char(s21_size_t *length, const char **str,
                    int *read_bytes_count);
bool get_input(const char **str, current_format *settings, void *dest,
               void **pointer_dest, int *read_bytes_count);

#endif