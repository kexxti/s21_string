#ifndef S21_SSCANF_H
#define S21_SSCANF_H

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>  // for EOF

#include "s21_string.h"

typedef enum SCAN_MODIFIER {
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
} SCAN_MODIFIER;
typedef enum SCAN_LENGTH { NONE, SHORT, LONG, EXTENDED_DOUBLE } SCAN_LENGTH;
typedef struct scan_format {
  bool is_symbol;        // if *char* in format without % then it is read as is,
  char matching_symbol;  // if *char* and the read one differ scan fails

  bool ignore_whitespaces;
  bool ignore;
  s21_size_t width;
  SCAN_LENGTH length_modifier;
  SCAN_MODIFIER type_modifier;
} scan_format;

int s21_sscanf(const char *str, const char *format, ...);
scan_format *init_scan_format();
void fill_scan_format_default(scan_format *settings);
void read_scan_format(const char **format, scan_format *settings);
SCAN_LENGTH get_scan_length(const char ch);
SCAN_MODIFIER get_scan_modifier(const char ch);
s21_size_t get_scan_width(const char **format);
void normalize_scan_format(scan_format *settings);
bool needs_scan_dest(scan_format *settings);
void *get_next_scan_dest(scan_format *settings, va_list args);
bool get_symbol(const char **str, scan_format *settings, int *read_bytes_count);
bool get_char(const char **str, scan_format *settings, void *dest,
              int *read_bytes_count);
bool is_negative_ato(s21_size_t *length, const char **str,
                     int *read_bytes_count);
void atoi_move_to_next(s21_size_t *length, const char **str,
                       int *read_bytes_count, bool *is_scan_succeded);
int s21_atoi(const char **str, scan_format *settings, int *read_bytes_count,
             bool *is_scan_succeded);
short int s21_atosi(const char **str, scan_format *settings,
                    int *read_bytes_count, bool *is_scan_succeded);
long int s21_atol(const char **str, scan_format *settings,
                  int *read_bytes_count, bool *is_scan_succeded);
bool get_decimal(const char **str, scan_format *settings, void *dest,
                 int *read_bytes_count);
void skip_whitespaces(const char **str, int *read_bytes_count);
bool get_udecimal(const char **str, scan_format *settings, void *dest,
                  int *read_bytes_count);
int s21_a_to_octali(const char **str, scan_format *settings,
                    int *read_bytes_count, bool *is_scan_succeded);
short int s21_a_to_octalsi(const char **str, scan_format *settings,
                           int *read_bytes_count, bool *is_scan_succeded);
long int s21_a_to_octall(const char **str, scan_format *settings,
                         int *read_bytes_count, bool *is_scan_succeded);
bool get_uoctal(const char **str, scan_format *settings, void *dest,
                int *read_bytes_count);
void skip_hex_prefix(s21_size_t *length, const char **str,
                     scan_format *settings, int *read_bytes_count);
bool skip_hex_atoi_prefixes(s21_size_t *length, const char **str,
                            scan_format *settings, int *read_bytes_count);
int s21_a_to_hexi(const char **str, scan_format *settings,
                  int *read_bytes_count, bool *is_scan_succeded);
short int s21_a_to_hexsi(const char **str, scan_format *settings,
                         int *read_bytes_count, bool *is_scan_succeded);
long int s21_a_to_hexl(const char **str, scan_format *settings,
                       int *read_bytes_count, bool *is_scan_succeded);
bool get_uhex(const char **str, scan_format *settings, void *dest,
              int *read_bytes_count);
bool get_integer(const char **str, scan_format *settings, void *dest,
                 int *read_bytes_count);
bool get_string(const char **str, scan_format *settings, void *dest,
                int *read_bytes_count);
bool get_number(const char **str, scan_format *settings, void *dest,
                int *read_bytes_count);
bool get_pointer(const char **str, scan_format *settings, void **dest,
                 int *read_bytes_count);
bool get_float(const char **str, scan_format *settings, void *dest,
               int *read_bytes_count);
long double read_extended_double(const char **str, scan_format *settings,
                                 int *read_bytes_count, bool *is_scan_succeded);

void goto_next_char(s21_size_t *length, const char **str,
                    int *read_bytes_count);
bool get_input(const char **str, scan_format *settings, void *dest,
               void **pointer_dest, int *read_bytes_count);

#endif