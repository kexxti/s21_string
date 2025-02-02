#include "test_s21_sscanf.h"

#include <stdlib.h>
#include <string.h>

#include "../string/s21_sscanf.h"
#include "../string/s21_string.h"

// what if "%*%" or broken like "%l" or "%"

static int compare_formats(current_format *format1, current_format *format2);

START_TEST(test_sscanf_get_length) {
  LENGTH res_none = get_length('C');
  ck_assert_uint_eq(res_none, NONE);
  LENGTH res_none_empty = get_length('\0');
  ck_assert_uint_eq(res_none_empty, NONE);
  LENGTH res_long = get_length('l');
  ck_assert_uint_eq(res_long, LONG);
  LENGTH res_short = get_length('h');
  ck_assert_uint_eq(res_short, SHORT);
  LENGTH res_none_upper = get_length('H');
  ck_assert_uint_eq(res_none_upper, NONE);
  LENGTH res_ext = get_length('L');
  ck_assert_uint_eq(res_ext, EXTENDED_DOUBLE);
}
END_TEST

START_TEST(test_sscanf_get_modifier) {
  MODIFIER res_err = get_modifier('C');
  ck_assert_uint_eq(res_err, ERR);
  MODIFIER res_err_empty = get_modifier('\0');
  ck_assert_uint_eq(res_err_empty, ERR);
  MODIFIER res_char = get_modifier('c');
  ck_assert_uint_eq(res_char, CHAR);
  MODIFIER res_decimal = get_modifier('d');
  ck_assert_uint_eq(res_decimal, DECIMAL);
  MODIFIER res_integer = get_modifier('i');
  ck_assert_uint_eq(res_integer, INTEGER);

  MODIFIER res_float_e = get_modifier('e');
  ck_assert_uint_eq(res_float_e, FLOAT);
  MODIFIER res_float_upper_e = get_modifier('E');
  ck_assert_uint_eq(res_float_upper_e, FLOAT);
  MODIFIER res_float_f = get_modifier('f');
  ck_assert_uint_eq(res_float_f, FLOAT);
  MODIFIER res_float_g = get_modifier('g');
  ck_assert_uint_eq(res_float_g, FLOAT);
  MODIFIER res_float_upper_g = get_modifier('G');
  ck_assert_uint_eq(res_float_upper_g, FLOAT);

  MODIFIER res_octal = get_modifier('o');
  ck_assert_uint_eq(res_octal, UOCTAL);
  MODIFIER res_string = get_modifier('s');
  ck_assert_uint_eq(res_string, STRING);
  MODIFIER res_udecimal = get_modifier('u');
  ck_assert_uint_eq(res_udecimal, UDECIMAL);

  MODIFIER res_uhex = get_modifier('x');
  ck_assert_uint_eq(res_uhex, UHEX);
  MODIFIER res_upper_uhex = get_modifier('X');
  ck_assert_uint_eq(res_upper_uhex, UHEX);

  MODIFIER res_pointer = get_modifier('p');
  ck_assert_uint_eq(res_pointer, POINTER);
  MODIFIER res_number = get_modifier('n');
  ck_assert_uint_eq(res_number, NUMBER);
  MODIFIER res_percent = get_modifier('%');
  ck_assert_uint_eq(res_percent, PERCENT);
}
END_TEST

START_TEST(test_sscanf_get_width) {
  const char *string_empty = "\0";
  s21_size_t res_empty = get_width(&string_empty);
  ck_assert_uint_eq(res_empty, 0);
  ck_assert_str_eq(string_empty, "\0");

  const char *string_none = "Ld";
  s21_size_t res_none = get_width(&string_none);
  ck_assert_uint_eq(res_none, 0);
  ck_assert_str_eq(string_none, "Ld");

  const char *string_zero = "0";
  s21_size_t res_zero = get_width(&string_zero);
  ck_assert_uint_eq(res_zero, 0);
  ck_assert_str_eq(string_zero, "\0");

  const char *string_zero_plus = "0u";
  s21_size_t res_zero_plus = get_width(&string_zero_plus);
  ck_assert_uint_eq(res_zero_plus, 0);
  ck_assert_str_eq(string_zero_plus, "u");

  const char *string_single_digit = "5i";
  s21_size_t res_single_digit = get_width(&string_single_digit);
  ck_assert_uint_eq(res_single_digit, 5);
  ck_assert_str_eq(string_single_digit, "i");

  const char *string_leading_zero = "07u";
  s21_size_t res_leading_zero = get_width(&string_leading_zero);
  ck_assert_uint_eq(res_leading_zero, 7);
  ck_assert_str_eq(string_leading_zero, "u");

  const char *string_two_digits = "15f";
  s21_size_t res_two_digits = get_width(&string_two_digits);
  ck_assert_uint_eq(res_two_digits, 15);
  ck_assert_str_eq(string_two_digits, "f");

  const char *string_two_digit_leading_zeros = "0015f";
  s21_size_t res_two_digit_leading_zeros =
      get_width(&string_two_digit_leading_zeros);
  ck_assert_uint_eq(res_two_digit_leading_zeros, 15);
  ck_assert_str_eq(string_two_digit_leading_zeros, "f");

  const char *string_three_digits = "683s";
  s21_size_t res_three_digits = get_width(&string_three_digits);
  ck_assert_uint_eq(res_three_digits, 683);
  ck_assert_str_eq(string_three_digits, "s");

  const char *string_zero_additional = "0s %f g";
  s21_size_t res_zero_additional = get_width(&string_zero_additional);
  ck_assert_uint_eq(res_zero_additional, 0);
  ck_assert_str_eq(string_zero_additional, "s %f g");

  const char *string_additional = "5s %7f 9g";
  s21_size_t res_additional = get_width(&string_additional);
  ck_assert_uint_eq(res_additional, 5);
  ck_assert_str_eq(string_additional, "s %7f 9g");
}
END_TEST

START_TEST(test_sscanf_formats) {
  current_format *f1 = init_format();
  current_format *f2 = init_format();
  const char *format = "%5s   %7f 9g  %*X%*9ld.o %%\n f";

  read_format(&format, f1);
  f2->width = 5;
  f2->type_modifier = STRING;
  int res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, "   %7f 9g  %*X%*9ld.o %%\n f");

  fill_format_default(f1);
  fill_format_default(f2);
  read_format(&format, f1);
  f2->ignore_whitespaces = true;
  f2->width = 7;
  f2->type_modifier = FLOAT;
  res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, " 9g  %*X%*9ld.o %%\n f");

  fill_format_default(f1);
  fill_format_default(f2);
  read_format(&format, f1);
  f2->ignore_whitespaces = true;
  f2->is_symbol = true;
  f2->matching_symbol = '9';
  res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, "g  %*X%*9ld.o %%\n f");

  fill_format_default(f1);
  fill_format_default(f2);
  read_format(&format, f1);
  f2->is_symbol = true;
  f2->matching_symbol = 'g';
  res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, "  %*X%*9ld.o %%\n f");

  fill_format_default(f1);
  fill_format_default(f2);
  read_format(&format, f1);
  f2->ignore_whitespaces = true;
  f2->ignore = true;
  f2->type_modifier = UHEX;
  res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, "%*9ld.o %%\n f");

  fill_format_default(f1);
  fill_format_default(f2);
  read_format(&format, f1);
  f2->ignore = true;
  f2->width = 9;
  f2->length_modifier = LONG;
  f2->type_modifier = DECIMAL;
  res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, ".o %%\n f");

  fill_format_default(f1);
  fill_format_default(f2);
  read_format(&format, f1);
  f2->is_symbol = true;
  f2->matching_symbol = '.';
  res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, "o %%\n f");

  fill_format_default(f1);
  fill_format_default(f2);
  read_format(&format, f1);
  f2->is_symbol = true;
  f2->matching_symbol = 'o';
  res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, " %%\n f");

  fill_format_default(f1);
  fill_format_default(f2);
  read_format(&format, f1);
  f2->ignore_whitespaces = true;
  f2->type_modifier = PERCENT;
  res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, "\n f");

  fill_format_default(f1);
  fill_format_default(f2);
  read_format(&format, f1);
  f2->ignore_whitespaces = true;
  f2->is_symbol = true;
  f2->matching_symbol = 'f';
  res = compare_formats(f1, f2);
  ck_assert_uint_eq(res, 0);
  ck_assert_str_eq(format, "\0");

  free(f1);
  free(f2);
}
END_TEST

START_TEST(test_default_sscanf) {
  int value = 0;
  int res = sscanf("123", "%d", &value);
  ck_assert_int_eq(value, 123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("123", " %d", &value);
  ck_assert_int_eq(value, 123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    123", "  %d", &value);
  ck_assert_int_eq(value, 123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    123", "\n%d", &value);
  ck_assert_int_eq(value, 123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("-123", "\n %d", &value);
  ck_assert_int_eq(value, -123);
  ck_assert_int_eq(res, 1);

  value = 1;
  int value2 = 8;
  res = sscanf("-", "%d%n", &value, &value2);
  ck_assert_int_eq(value, 1);
  ck_assert_int_eq(value2, 8);
  ck_assert_int_eq(res, 0);

  // octal
  value = 0;
  res = sscanf("0123", " %o", &value);
  ck_assert_int_eq(value, 0123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    0123", "  %o", &value);
  ck_assert_int_eq(value, 0123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    0123", "\n%o", &value);
  ck_assert_int_eq(value, 0123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("-0123", "\n %o", &value);
  ck_assert_int_eq(value, -0123);
  ck_assert_int_eq(res, 1);

  // octal no zero in beginning
  value = 0;
  res = sscanf("123", " %o", &value);
  ck_assert_int_eq(value, 0123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    123", "  %o", &value);
  ck_assert_int_eq(value, 0123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    123", "\n%o", &value);
  ck_assert_int_eq(value, 0123);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("-123", "\n %o", &value);
  ck_assert_int_eq(value, -0123);
  ck_assert_int_eq(res, 1);

  // hex
  value = 0;
  res = sscanf("0x12fa3", " %X", &value);
  ck_assert_int_eq(value, 0x12fa3);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    0x12fa3", "  %X", &value);
  ck_assert_int_eq(value, 0x12fa3);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    0x12fa3", "\n%X", &value);
  ck_assert_int_eq(value, 0x12fa3);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("-0x12fa3", "\n %X", &value);
  ck_assert_int_eq(value, -0x12fa3);
  ck_assert_int_eq(res, 1);

  value = 1;
  res = sscanf("x12fa3", " %X", &value);
  ck_assert_int_eq(value, 1);
  ck_assert_int_eq(res, 0);

  // hex without prefix
  value = 0;
  res = sscanf("12fa3", " %X", &value);
  ck_assert_int_eq(value, 0x12fa3);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    12fa3", "  %X", &value);
  ck_assert_int_eq(value, 0x12fa3);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("    12fa3", "\n%X", &value);
  ck_assert_int_eq(value, 0x12fa3);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("-12fa3", "\n %X", &value);
  ck_assert_int_eq(value, -0x12fa3);
  ck_assert_int_eq(res, 1);

  // what? why?
  value = 1;
  res = sscanf("0", "\n %o", &value);
  ck_assert_int_eq(value, 0);
  ck_assert_int_eq(res, 1);

  value = 1;
  res = sscanf("0x", "\n %X", &value);
  ck_assert_int_eq(value, 0x0);
  ck_assert_int_eq(res, 1);

  // empty does nothing
  value = 1;
  res = sscanf("\0", "%d", &value);
  ck_assert_int_eq(value, 1);
  ck_assert_int_eq(res, EOF);

  // general integer
  value = 0;
  res = sscanf("123", "%i", &value);
  ck_assert_int_eq(value, 123);
  ck_assert_int_eq(res, 1);
  res = sscanf("054", "%i", &value);
  ck_assert_int_eq(value, 054);
  ck_assert_int_eq(res, 1);
  res = sscanf("0xFF", "%i", &value);
  ck_assert_int_eq(value, 0xFF);
  ck_assert_int_eq(res, 1);

  // chars
  char ch1, ch2, ch3;
  res = sscanf("abc", "%c%c%c", &ch1, &ch2, &ch3);
  ck_assert_int_eq(ch1, 'a');
  ck_assert_int_eq(ch2, 'b');
  ck_assert_int_eq(ch3, 'c');
  ck_assert_int_eq(res, 3);

  ch1 = ch2 = ch3 = '\0';
  res = sscanf(" abc", "%c%c%c", &ch1, &ch2, &ch3);
  ck_assert_int_eq(ch1, ' ');
  ck_assert_int_eq(ch2, 'a');
  ck_assert_int_eq(ch3, 'b');
  ck_assert_int_eq(res, 3);

  ch1 = ch2 = ch3 = '\0';
  res = sscanf(" a   bc", " %c\n%c %c", &ch1, &ch2, &ch3);
  ck_assert_int_eq(ch1, 'a');
  ck_assert_int_eq(ch2, 'b');
  ck_assert_int_eq(ch3, 'c');
  ck_assert_int_eq(res, 3);

  ch1 = ch2 = '\0';
  res = sscanf("a", "%c%c", &ch1, &ch2);  // NULL_TERMINATOR
  ck_assert_int_eq(ch1, 'a');
  ck_assert_int_eq(ch2, '\0');
  ck_assert_int_eq(res, 1);

  // hex and octal with OOB
  value = 0;
  int remain = 0;
  res = sscanf("0249", "%o%d", &value, &remain);
  ck_assert_int_eq(value, 024);
  ck_assert_int_eq(remain, 9);
  ck_assert_int_eq(res, 2);
  value = 0;
  ch1 = '\0';
  res = sscanf("0x249fh", "%x%c", &value, &ch1);
  ck_assert_int_eq(value, 0x249f);
  ck_assert_int_eq(ch1, 'h');
  ck_assert_int_eq(res, 2);

  // char as dec | no result
  value = 1;
  res = sscanf("h", "%d", &value);
  ck_assert_int_eq(value, 1);
  ck_assert_int_eq(res, 0);

  // unsigned
  value = 0;
  res = sscanf("2147483648", "%u", &value);
  ck_assert_int_eq(value, -2147483648);
  ck_assert_int_eq(res, 1);

  value = 1;
  res = sscanf("-10", "%u", &value);  // FOR SOME REASON IT READS AS IS
  ck_assert_int_eq(value, -10);
  ck_assert_int_eq(res, 1);

  uint32_t uval = 1;
  res = sscanf("-10", "%u", &uval);  // kinda funny
  ck_assert_int_eq(uval, 4294967286);
  ck_assert_int_ne(uval, -10);
  ck_assert_uint_eq(uval, 4294967286);
  ck_assert_uint_ne(uval, -10);
  ck_assert_int_eq(res, 1);

  // floats
  float fval = 0;
  res = sscanf("0.1", "%e", &fval);
  ck_assert_float_eq(fval, 0.1);
  ck_assert_int_eq(res, 1);
  fval = 0;
  res = sscanf("0.1", "%E", &fval);
  ck_assert_float_eq(fval, 0.1);
  ck_assert_int_eq(res, 1);
  fval = 0;
  res = sscanf("0.1", "%f", &fval);
  ck_assert_float_eq(fval, 0.1);
  ck_assert_int_eq(res, 1);
  fval = 0;
  res = sscanf("0.1", "%g", &fval);
  ck_assert_float_eq(fval, 0.1);
  ck_assert_int_eq(res, 1);
  fval = 0;
  res = sscanf("0.1", "%G", &fval);
  ck_assert_float_eq(fval, 0.1);
  ck_assert_int_eq(res, 1);

  fval = 0;
  res = sscanf("1.2E7", "%f", &fval);
  ck_assert_float_eq(fval, 1.2E7);
  ck_assert_int_eq(res, 1);
  fval = 0;
  res = sscanf("1E0", "%f", &fval);
  ck_assert_float_eq(fval, 1E0);
  ck_assert_int_eq(res, 1);
  fval = 0;
  res = sscanf("-1E-5", "%f", &fval);
  ck_assert_float_eq(fval, -1E-5);
  ck_assert_int_eq(res, 1);

  fval = 0;
  res = sscanf("1E", "%f", &fval);  // not exponential
  ck_assert_float_eq(fval, 1);
  ck_assert_int_eq(res, 1);
  fval = 0;
  res = sscanf(".1e3", "%f", &fval);  // period in beggining
  ck_assert_float_eq(fval, .1e3);
  ck_assert_int_eq(res, 1);
  fval = 1;
  res = sscanf("0E1", "%f", &fval);
  ck_assert_float_eq(fval, 0);
  ck_assert_int_eq(res, 1);
  fval = 1;
  res = sscanf("12.3E2", "%f", &fval);  // not scientific
  ck_assert_float_eq(fval, 1230);
  ck_assert_int_eq(res, 1);
  fval = 1;
  res = sscanf("1.2E128", "%f", &fval);  // loss of meaning
  ck_assert_float_eq(fval, INFINITY);
  ck_assert_int_eq(res, 1);
  fval = 1;
  res = sscanf("1.2E-128", "%f", &fval);  // loss of meaning
  ck_assert_float_eq(fval, 0);
  ck_assert_int_eq(res, 1);

  // double

  double dval = 1;
  res = sscanf("1.2E128", "%lf", &dval);
  ck_assert_double_eq(dval, 1.2E128);
  ck_assert_double_ne(dval, INFINITY);
  ck_assert_int_eq(res, 1);

  fval = 1;
  dval = 1;  // REALLY
  res = sscanf("7.51918915111644124123412412343", "%f", &fval);
  ck_assert_int_eq(res, 1);
  res = sscanf("7.51918915111644124123412412343", "%lf", &dval);
  ck_assert_int_eq(res, 1);
  ck_assert_double_ne(dval, fval);

  // ext float
  if (sizeof(double) != sizeof(long double)) {
    long double ldval = 1;
    dval = 1;
    const char *really_long_float_string =
        "5.5191891511164412412341241234351918915111644124123412412343";
    res = sscanf(really_long_float_string, "%Lf", &ldval);
    ck_assert_int_eq(res, 1);
    res = sscanf(really_long_float_string, "%lf", &dval);
    ck_assert_int_eq(res, 1);
    ck_assert_ldouble_ne(ldval, dval);
  }
  // extra
  fval = 1;
  res = sscanf(".02.3", "%*f%f", &fval);
  ck_assert_int_eq(res, 1);
  ck_assert_float_eq(fval, 0.3);

  // strings
  char str[100];
  const char *text = "Lorem \n  ipsum dolor sit amet";
  res = sscanf(text, "%s", str);
  ck_assert_int_eq(res, 1);
  ck_assert_str_eq(str, "Lorem");
  str[0] = '\0';
  res = sscanf(text, "%20s", str);
  ck_assert_int_eq(res, 1);
  ck_assert_str_eq(str, "Lorem");
  str[0] = '\0';
  res = sscanf(text, "%3s", str);
  ck_assert_int_eq(res, 1);
  ck_assert_str_eq(str, "Lor");

  str[0] = '\0';
  res = sscanf(text, "%*s%s", str);  // ignoring whitespaces anyway
  ck_assert_int_eq(res, 1);
  ck_assert_str_eq(str, "ipsum");

  str[0] = '\0';
  res = sscanf("Lorem", "%s%s", str, str);
  ck_assert_int_eq(res, 1);
  ck_assert_str_eq(str, "Lorem");

  // pointers
  void *pp2 = (void *)0x80af1465;
  void *pp1;
  res = sscanf("0x80af1465", "%p", &pp1);
  ck_assert_ptr_eq(pp1, pp2);
  ck_assert_int_eq(res, 1);

  pp2 = NULL;
  res = sscanf("0", "%p", &pp1);
  ck_assert_ptr_eq(pp1, pp2);
  ck_assert_int_eq(res, 1);

  pp2 = (void *)0xFFFFFFFF;
  res = sscanf("0xFFFFFFFF", "%p", &pp1);
  ck_assert_ptr_eq(pp1, pp2);
  ck_assert_int_eq(res, 1);

  pp2 = pp1 = (void *)1;
  res = sscanf("-0x1", "%p", &pp1);  // yes it works
  ck_assert_ptr_ne(pp1, pp2);
  ck_assert_int_eq(res, 1);

  // number
  ch1 = ch2 = ch3 = '\0';
  res = sscanf(" a   bc", " %c\n%c%n%c", &ch1, &ch2, &value, &ch3);
  ck_assert_int_eq(ch1, 'a');
  ck_assert_int_eq(ch2, 'b');
  ck_assert_int_eq(ch3, 'c');
  ck_assert_int_eq(value, 6);  // counts not written values but read chars (even
                               // ignored whitespaces)
  ck_assert_int_eq(res, 3);    // and doesn't add to overall counter

  ch1 = ch2 = ch3 = '\0';
  res = sscanf(" a   bc", "%n %c\n%c%c", &value, &ch1, &ch2, &ch3);
  ck_assert_int_eq(ch1, 'a');
  ck_assert_int_eq(ch2, 'b');
  ck_assert_int_eq(ch3, 'c');
  ck_assert_int_eq(value, 0);  // not EOF by default
  ck_assert_int_eq(res, 3);

  ch1 = ch2 = ch3 = '\0';
  res = sscanf("  a   bc", " %n %c\n%c%c", &value, &ch1, &ch2, &ch3);
  ck_assert_int_eq(ch1, 'a');
  ck_assert_int_eq(ch2, 'b');
  ck_assert_int_eq(ch3, 'c');
  ck_assert_int_eq(value, 2);  // but for some reason counts skipped whitespaces
  ck_assert_int_eq(res, 3);

  ch1 = ch2 = ch3 = '\0';
  res = sscanf("a      ", "%c%c%c%n", &ch1, &ch2, &ch3, &value);
  ck_assert_int_eq(ch1, 'a');
  ck_assert_int_eq(ch2, ' ');
  ck_assert_int_eq(ch3, ' ');
  ck_assert_int_eq(value, 3);  // ^^^ and only if they are skipped ^^^
  ck_assert_int_eq(res, 3);

  // symbols and percent
  ch1 = ch2 = ch3 = '\0';
  str[0] = '\0';
  res = sscanf("Lorem \n\t  ipsum% dolor sit amet", "%*s\tipsum%%%s", str);
  ck_assert_str_eq(str, "dolor");
  ck_assert_int_eq(res, 1);

  // limits
  value = 0;
  res = sscanf("-2147483648", "%d", &value);
  ck_assert_int_eq(value, INT32_MIN);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("2147483647", "%d", &value);
  ck_assert_int_eq(value, INT32_MAX);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("2147483648", "%d", &value);
  ck_assert_int_eq(value, INT32_MIN);
  ck_assert_int_eq(res, 1);

  value = 0;
  res = sscanf("-2147483649", "%d", &value);
  ck_assert_int_eq(value, INT32_MAX);
  ck_assert_int_eq(res, 1);

  // whitespaces skipping
  value = value2 = 0;
  res = sscanf("  123   \n\t546", "%d%d", &value, &value2);
  ck_assert_int_eq(value, 123);
  ck_assert_int_eq(value2, 546);
  ck_assert_int_eq(res, 2);
}
END_TEST

START_TEST(test_custom_atoi) {
  char str[100];
  char *str2;
  strcpy(str, "123");
  str2 = strdup(str);
  char *temp = str2;
  int value = s21_atoi((const char **)&str2, NULL, NULL);
  int value2 = atoi(str);
  ck_assert_int_eq(value, 123);
  ck_assert_int_eq(value, value2);
  free(temp);

  strcpy(str, "-123");
  str2 = strdup(str);
  temp = str2;
  value = s21_atoi((const char **)&str2, NULL, NULL);
  value2 = atoi(str);
  ck_assert_int_eq(value, -123);
  ck_assert_int_eq(value, value2);
  free(temp);

  strcpy(str, "00123");
  str2 = strdup(str);
  temp = str2;
  value = s21_atoi((const char **)&str2, NULL, NULL);
  value2 = atoi(str);
  ck_assert_int_eq(value, 123);
  ck_assert_int_eq(value, value2);
  free(temp);

  strcpy(str, "-00123");
  str2 = strdup(str);
  temp = str2;
  value = s21_atoi((const char **)&str2, NULL, NULL);
  value2 = atoi(str);
  ck_assert_int_eq(value, -123);
  ck_assert_int_eq(value, value2);
  free(temp);

  strcpy(str, "2147483647");
  str2 = strdup(str);
  temp = str2;
  value = s21_atoi((const char **)&str2, NULL, NULL);
  value2 = atoi(str);
  ck_assert_int_eq(value, INT32_MAX);
  ck_assert_int_eq(value, value2);
  free(temp);

  strcpy(str, "-2147483648");
  str2 = strdup(str);
  temp = str2;
  value = s21_atoi((const char **)&str2, NULL, NULL);
  value2 = atoi(str);
  ck_assert_int_eq(value, INT32_MIN);
  ck_assert_int_eq(value, value2);
  free(temp);

  strcpy(str, "-2147483649");
  str2 = strdup(str);
  temp = str2;
  value = s21_atoi((const char **)&str2, NULL, NULL);
  value2 = atoi(str);
  ck_assert_int_eq(value, INT32_MAX);
  ck_assert_int_eq(value, value2);
  free(temp);

  strcpy(str, "2147483648");
  str2 = strdup(str);
  temp = str2;
  value = s21_atoi((const char **)&str2, NULL, NULL);
  value2 = atoi(str);
  ck_assert_int_eq(value, INT32_MIN);
  ck_assert_int_eq(value, value2);
  free(temp);

  strcpy(str, "781235671236758");
  str2 = strdup(str);
  temp = str2;
  value = s21_atoi((const char **)&str2, NULL, NULL);
  value2 = atoi(str);
  ck_assert_int_eq(value, value2);
  free(temp);
}
END_TEST

START_TEST(test_s21_sscanf) {
  int value_def = 0;
  int res_def = sscanf("123", "%d", &value_def);
  int value_s21 = 0;
  int res_s21 = s21_sscanf("123", "%d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 123);
  ck_assert_int_eq(value_s21, 123);

  value_def = value_s21 = 0;
  res_def = sscanf("123", " %d", &value_def);
  res_s21 = s21_sscanf("123", " %d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 123);
  ck_assert_int_eq(value_s21, 123);

  value_def = value_s21 = 0;
  res_def = sscanf("    123", "  %d", &value_def);
  res_s21 = s21_sscanf("    123", "  %d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 123);
  ck_assert_int_eq(value_s21, 123);

  value_def = value_s21 = 0;
  res_def = sscanf("    123", "\n%d", &value_def);
  res_s21 = s21_sscanf("    123", "\n%d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 123);
  ck_assert_int_eq(value_s21, 123);

  value_def = value_s21 = 0;
  res_def = sscanf("-123", "\n %d", &value_def);
  res_s21 = s21_sscanf("-123", "\n %d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, -123);
  ck_assert_int_eq(value_s21, -123);

  value_def = value_s21 = 8;
  res_def = sscanf("-", "%d", &value_def);
  res_s21 = s21_sscanf("-", "%d", &value_s21);
  ck_assert_int_eq(res_def, 0);
  ck_assert_int_eq(res_s21, 0);
  ck_assert_int_eq(value_def, 8);
  ck_assert_int_eq(value_s21, 8);

  // char as dec | no result
  value_def = value_s21 = 1;
  res_def = sscanf("h", "%d", &value_def);
  res_s21 = s21_sscanf("h", "%d", &value_s21);
  ck_assert_int_eq(res_def, 0);
  ck_assert_int_eq(res_s21, 0);
  ck_assert_int_eq(value_def, 1);
  ck_assert_int_eq(value_s21, 1);

  // limits
  value_def = value_s21 = 0;
  res_def = sscanf("-2147483648", "%d", &value_def);
  res_s21 = s21_sscanf("-2147483648", "%d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, INT32_MIN);
  ck_assert_int_eq(value_s21, INT32_MIN);

  value_def = value_s21 = 0;
  res_def = sscanf("2147483647", "%d", &value_def);
  res_s21 = s21_sscanf("2147483647", "%d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, INT32_MAX);
  ck_assert_int_eq(value_s21, INT32_MAX);

  value_def = value_s21 = 0;
  res_def = sscanf("2147483648", "%d", &value_def);
  res_s21 = s21_sscanf("2147483648", "%d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, INT32_MIN);
  ck_assert_int_eq(value_s21, INT32_MIN);

  value_def = value_s21 = 0;
  res_def = sscanf("-2147483649", "%d", &value_def);
  res_s21 = s21_sscanf("-2147483649", "%d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, INT32_MAX);
  ck_assert_int_eq(value_s21, INT32_MAX);

  // symbols and percent
  char ch1_def, ch2_def, ch3_def;
  char ch1_s21, ch2_s21, ch3_s21;
  ch1_def = ch2_def = ch3_def = '\0';
  ch1_s21 = ch2_s21 = ch3_s21 = '\0';
  res_def = sscanf("abc", "%c%c%c", &ch1_def, &ch2_def, &ch3_def);
  res_s21 = s21_sscanf("abc", "%c%c%c", &ch1_s21, &ch2_s21, &ch3_s21);
  ck_assert_int_eq(res_def, 3);
  ck_assert_int_eq(res_s21, 3);
  ck_assert_int_eq(ch1_def, 'a');
  ck_assert_int_eq(ch2_def, 'b');
  ck_assert_int_eq(ch3_def, 'c');
  ck_assert_int_eq(ch1_s21, 'a');
  ck_assert_int_eq(ch2_s21, 'b');
  ck_assert_int_eq(ch3_s21, 'c');

  ch1_def = ch2_def = ch3_def = '\0';
  ch1_s21 = ch2_s21 = ch3_s21 = '\0';
  res_def = sscanf(" abc", "%c%c%c", &ch1_def, &ch2_def, &ch3_def);
  res_s21 = s21_sscanf(" abc", "%c%c%c", &ch1_s21, &ch2_s21, &ch3_s21);
  ck_assert_int_eq(res_def, 3);
  ck_assert_int_eq(res_s21, 3);
  ck_assert_int_eq(ch1_def, ' ');
  ck_assert_int_eq(ch2_def, 'a');
  ck_assert_int_eq(ch3_def, 'b');
  ck_assert_int_eq(ch1_s21, ' ');
  ck_assert_int_eq(ch2_s21, 'a');
  ck_assert_int_eq(ch3_s21, 'b');

  ch1_def = ch2_def = ch3_def = '\0';
  ch1_s21 = ch2_s21 = ch3_s21 = '\0';
  res_def = sscanf(" a   bc", " %c\n%c %c", &ch1_def, &ch2_def, &ch3_def);
  res_s21 = s21_sscanf(" a   bc", " %c\n%c %c", &ch1_s21, &ch2_s21, &ch3_s21);
  ck_assert_int_eq(res_def, 3);
  ck_assert_int_eq(res_s21, 3);
  ck_assert_int_eq(ch1_def, 'a');
  ck_assert_int_eq(ch2_def, 'b');
  ck_assert_int_eq(ch3_def, 'c');
  ck_assert_int_eq(ch1_s21, 'a');
  ck_assert_int_eq(ch2_s21, 'b');
  ck_assert_int_eq(ch3_s21, 'c');

  ch1_def = ch2_def = '\0';
  ch1_s21 = ch2_s21 = '\0';
  res_def = sscanf("a", "%c%c", &ch1_def, &ch2_def);  // NULL_TERMINATOR
  res_s21 = s21_sscanf("a", "%c%c", &ch1_s21, &ch2_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(ch1_def, 'a');
  ck_assert_int_eq(ch2_def, '\0');
  ck_assert_int_eq(ch1_s21, 'a');
  ck_assert_int_eq(ch2_s21, '\0');

  // symbols
  value_def = value_s21 = 0;
  res_def = sscanf("some    words1234", "some\nwords%d", &value_def);
  res_s21 = s21_sscanf("some    words1234", "some\nwords%d", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 1234);
  ck_assert_int_eq(value_s21, 1234);

  // unsigned
  value_def = value_s21 = 0;
  res_def = sscanf("2147483648", "%u", &value_def);
  res_s21 = s21_sscanf("2147483648", "%u", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, -2147483648);
  ck_assert_int_eq(value_s21, -2147483648);

  value_def = value_s21 = 1;
  res_def = sscanf("-10", "%u", &value_def);
  res_s21 = s21_sscanf("-10", "%u", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, -10);
  ck_assert_int_eq(value_s21, -10);

  uint32_t uval_def = 1;
  uint32_t uval_s21 = 1;
  res_def = sscanf("-10", "%u", &uval_def);
  res_s21 = s21_sscanf("-10", "%u", &uval_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_uint_eq(uval_def, 4294967286);
  ck_assert_uint_eq(uval_s21, 4294967286);
  ck_assert_int_eq(uval_def, 4294967286);
  ck_assert_int_eq(uval_s21, 4294967286);
  ck_assert_int_ne(uval_def, -10);
  ck_assert_int_ne(uval_s21, -10);
  ck_assert_uint_ne(uval_def, -10);
  ck_assert_uint_ne(uval_s21, -10);

  value_def = value_s21 = 0;
  res_def = sscanf("   945462634324254", "\t%u", &value_def);
  res_s21 = s21_sscanf("   945462634324254", "\t%u", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, (int)945462634324254);
  ck_assert_int_eq(value_s21, (int)945462634324254);

  // short unsigned
  unsigned short int ushort_value_def, ushort_value_s21;
  ushort_value_def = ushort_value_s21 = 0;
  res_def = sscanf("1234", "%hu", &ushort_value_def);
  res_s21 = s21_sscanf("1234", "%hu", &ushort_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(ushort_value_def, 1234);
  ck_assert_int_eq(ushort_value_s21, 1234);

  ushort_value_def = ushort_value_s21 = 0;
  res_def = sscanf("    65535", "  %hu", &ushort_value_def);
  res_s21 = s21_sscanf("    65535", "  %hu", &ushort_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_uint_eq(ushort_value_def, 65535);
  ck_assert_uint_eq(ushort_value_s21, 65535);

  ushort_value_def = ushort_value_s21 = 0;
  res_def = sscanf("891235891235879", "%hu", &ushort_value_def);
  res_s21 = s21_sscanf("891235891235879", "%hu", &ushort_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_uint_eq(ushort_value_def, (unsigned short int)891235891235879);
  ck_assert_uint_eq(ushort_value_s21, (unsigned short int)891235891235879);

  // long unsigned
  unsigned long int ulong_value_def, ulong_value_s21;
  ulong_value_def = ulong_value_s21 = 0;
  res_def = sscanf("1234", "%lu", &ulong_value_def);
  res_s21 = s21_sscanf("1234", "%lu", &ulong_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(ulong_value_def, 1234);
  ck_assert_int_eq(ulong_value_s21, 1234);

  ulong_value_def = ulong_value_s21 = 0;
  res_def = sscanf("    4294967295", "  %lu", &ulong_value_def);
  res_s21 = s21_sscanf("    4294967295", "  %lu", &ulong_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_uint_eq(ulong_value_def, 4294967295);
  ck_assert_uint_eq(ulong_value_s21, 4294967295);

  ulong_value_def = ulong_value_s21 = 0;
  res_def = sscanf("891235891235879", "%lu", &ulong_value_def);
  res_s21 = s21_sscanf("891235891235879", "%lu", &ulong_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_uint_eq(ulong_value_def, 891235891235879);
  ck_assert_uint_eq(ulong_value_s21, 891235891235879);

  // octal
  value_def = value_s21 = 0;
  res_def = sscanf("0123", " %o", &value_def);
  res_s21 = s21_sscanf("0123", " %o", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0123);
  ck_assert_int_eq(value_s21, 0123);

  value_def = value_s21 = 0;
  res_def = sscanf("    0123", "  %o", &value_def);
  res_s21 = s21_sscanf("    0123", "  %o", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0123);
  ck_assert_int_eq(value_s21, 0123);

  value_def = value_s21 = 0;
  res_def = sscanf("    0123", "\n%o", &value_def);
  res_s21 = s21_sscanf("    0123", "\n%o", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0123);
  ck_assert_int_eq(value_s21, 0123);

  value_def = value_s21 = 0;
  res_def = sscanf("-0123", "\n %o", &value_def);
  res_s21 = s21_sscanf("-0123", "\n %o", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, -0123);
  ck_assert_int_eq(value_s21, -0123);

  // octal no zero in beginning
  value_def = value_s21 = 0;
  res_def = sscanf("123", " %o", &value_def);
  res_s21 = s21_sscanf("123", " %o", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0123);
  ck_assert_int_eq(value_s21, 0123);

  value_def = value_s21 = 0;
  res_def = sscanf("    123", "  %o", &value_def);
  res_s21 = s21_sscanf("    123", "  %o", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0123);
  ck_assert_int_eq(value_s21, 0123);

  value_def = value_s21 = 0;
  res_def = sscanf("    123", "\n%o", &value_def);
  res_s21 = s21_sscanf("    123", "\n%o", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0123);
  ck_assert_int_eq(value_s21, 0123);

  value_def = value_s21 = 0;
  res_def = sscanf("-123", "\n %o", &value_def);
  res_s21 = s21_sscanf("-123", "\n %o", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, -0123);
  ck_assert_int_eq(value_s21, -0123);

  // hex and octal with OOB

  int remain_def, remain_s21;
  value_def = value_s21 = 0;
  remain_def = remain_s21 = 0;
  res_def = sscanf("0249", "%o%d", &value_def, &remain_def);
  res_s21 = s21_sscanf("0249", "%o%d", &value_s21, &remain_s21);
  ck_assert_int_eq(res_def, 2);
  ck_assert_int_eq(res_s21, 2);
  ck_assert_int_eq(value_def, 024);
  ck_assert_int_eq(value_s21, 024);
  ck_assert_int_eq(remain_def, 9);
  ck_assert_int_eq(remain_s21, 9);

  value_def = value_s21 = 0;
  ch1_def = ch1_s21 = '\0';
  res_def = sscanf("0x249fh", "%x%c", &value_def, &ch1_def);
  res_s21 = s21_sscanf("0x249fh", "%x%c", &value_s21, &ch1_s21);
  ck_assert_int_eq(res_def, 2);
  ck_assert_int_eq(res_s21, 2);
  ck_assert_int_eq(value_def, 0x249f);
  ck_assert_int_eq(value_s21, 0x249f);
  ck_assert_int_eq(ch1_def, 'h');
  ck_assert_int_eq(ch1_s21, 'h');

  // hex
  value_def = value_s21 = 0;
  res_def = sscanf("0x12fa3", " %x", &value_def);
  res_s21 = s21_sscanf("0x12fa3", " %x", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0x12fa3);
  ck_assert_int_eq(value_s21, 0x12fa3);

  value_def = value_s21 = 0;
  res_def = sscanf("    0x12fa3", "  %X", &value_def);
  res_s21 = s21_sscanf("    0x12fa3", "  %X", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0x12fa3);
  ck_assert_int_eq(value_s21, 0x12fa3);

  value_def = value_s21 = 0;
  res_def = sscanf("    0x12fa3", "\n%X", &value_def);
  res_s21 = s21_sscanf("    0x12fa3", "\n%X", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0x12fa3);
  ck_assert_int_eq(value_s21, 0x12fa3);

  value_def = value_s21 = 0;
  res_def = sscanf("-0x12fa3", "\n %X", &value_def);
  res_s21 = s21_sscanf("-0x12fa3", "\n %X", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, -0x12fa3);
  ck_assert_int_eq(value_s21, -0x12fa3);

  value_def = value_s21 = 1;
  res_def = sscanf("x12fa3", " %X", &value_def);
  res_s21 = s21_sscanf("x12fa3", " %X", &value_s21);
  ck_assert_int_eq(value_def, 1);
  ck_assert_int_eq(value_s21, 1);
  ck_assert_int_eq(res_def, 0);
  ck_assert_int_eq(res_s21, 0);

  // hex without prefix
  value_def = value_s21 = 0;
  res_def = sscanf("12fa3", " %X", &value_def);
  res_s21 = s21_sscanf("12fa3", " %X", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0x12fa3);
  ck_assert_int_eq(value_s21, 0x12fa3);

  value_def = value_s21 = 0;
  res_def = sscanf("    12fa3", "  %X", &value_def);
  res_s21 = s21_sscanf("    12fa3", "  %X", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0x12fa3);
  ck_assert_int_eq(value_s21, 0x12fa3);

  value_def = value_s21 = 0;
  res_def = sscanf("    12fa3", "\n%X", &value_def);
  res_s21 = s21_sscanf("    12fa3", "\n%X", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0x12fa3);
  ck_assert_int_eq(value_s21, 0x12fa3);

  value_def = value_s21 = 0;
  res_def = sscanf("-12fa3", "\n %X", &value_def);
  res_s21 = s21_sscanf("-12fa3", "\n %X", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, -0x12fa3);
  ck_assert_int_eq(value_s21, -0x12fa3);

  // what? why?
  value_def = value_s21 = 1;
  res_def = sscanf("0", "\n %o", &value_def);
  res_s21 = s21_sscanf("0", "\n %o", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0);
  ck_assert_int_eq(value_s21, 0);

  value_def = value_s21 = 1;
  res_def = sscanf("0x", "\n %X", &value_def);
  res_s21 = s21_sscanf("0x", "\n %X", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0x0);
  ck_assert_int_eq(value_s21, 0x0);

  // general integer
  value_def = value_s21 = 0;
  res_def = sscanf("123", " %i", &value_def);
  res_s21 = s21_sscanf("123", " %i", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 123);
  ck_assert_int_eq(value_s21, 123);

  value_def = value_s21 = 0;
  res_def = sscanf("-123", " %i", &value_def);
  res_s21 = s21_sscanf("-123", " %i", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, -123);
  ck_assert_int_eq(value_s21, -123);

  value_def = value_s21 = 0;
  res_def = sscanf("054", "%i", &value_def);
  res_s21 = s21_sscanf("054", "%i", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 054);
  ck_assert_int_eq(value_s21, 054);

  value_def = value_s21 = 0;
  res_def = sscanf("0xFF", "%i", &value_def);
  res_s21 = s21_sscanf("0xFF", "%i", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0xFF);
  ck_assert_int_eq(value_s21, 0xFF);

  value_def = value_s21 = 1;
  res_def = sscanf("0x", "%i", &value_def);
  res_s21 = s21_sscanf("0x", "%i", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0x0);
  ck_assert_int_eq(value_s21, 0x0);

  value_def = value_s21 = 1;
  res_def = sscanf("0", "%i", &value_def);
  res_s21 = s21_sscanf("0", "%i", &value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(value_def, 0);
  ck_assert_int_eq(value_s21, 0);

  // percent
  value_def = value_s21 = 0;
  ch1_def = ch1_s21 = 0;
  res_def = sscanf("r%-1234", "%c%%%d", &ch1_def, &value_def);
  res_s21 = s21_sscanf("r%-1234", "%c%%%d", &ch1_s21, &value_s21);
  ck_assert_int_eq(res_def, 2);
  ck_assert_int_eq(res_s21, 2);
  ck_assert_int_eq(ch1_def, 'r');
  ck_assert_int_eq(ch1_s21, 'r');
  ck_assert_int_eq(value_def, -1234);
  ck_assert_int_eq(value_s21, -1234);

  // short ints
  short int short_value_def, short_value_s21;
  short_value_def = short_value_s21 = 0;
  res_def = sscanf("123", " %hi", &short_value_def);
  res_s21 = s21_sscanf("123", " %hi", &short_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(short_value_def, 123);
  ck_assert_int_eq(short_value_s21, 123);

  short_value_def = short_value_s21 = 0;
  res_def = sscanf("-123", " %hi", &short_value_def);
  res_s21 = s21_sscanf("-123", " %hi", &short_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(short_value_def, -123);
  ck_assert_int_eq(short_value_s21, -123);

  short_value_def = short_value_s21 = 0;
  res_def = sscanf("054", "%hi", &short_value_def);
  res_s21 = s21_sscanf("054", "%hi", &short_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(short_value_def, 054);
  ck_assert_int_eq(short_value_s21, 054);

  short_value_def = short_value_s21 = 0;
  res_def = sscanf("0xFF", "%hi", &short_value_def);
  res_s21 = s21_sscanf("0xFF", "%hi", &short_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(short_value_def, 0xFF);
  ck_assert_int_eq(short_value_s21, 0xFF);

  short_value_def = short_value_s21 = 0;
  res_def = sscanf("0x4F12F", "%hi", &short_value_def);
  res_s21 = s21_sscanf("0x4F12F", "%hi", &short_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(short_value_def, (short int)0x4F12F);
  ck_assert_int_eq(short_value_s21, (short int)0x4F12F);

  short_value_def = short_value_s21 = 1;
  res_def = sscanf("0x", "%hi", &short_value_def);
  res_s21 = s21_sscanf("0x", "%hi", &short_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(short_value_def, 0x0);
  ck_assert_int_eq(short_value_s21, 0x0);

  short_value_def = short_value_s21 = 1;
  res_def = sscanf("0", "%hi", &short_value_def);
  res_s21 = s21_sscanf("0", "%hi", &short_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(short_value_def, 0);
  ck_assert_int_eq(short_value_s21, 0);

  // long ints
  long int long_value_def, long_value_s21;
  long_value_def = long_value_s21 = 0;
  res_def = sscanf("123", " %li", &long_value_def);
  res_s21 = s21_sscanf("123", " %li", &long_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(long_value_def, 123);
  ck_assert_int_eq(long_value_s21, 123);

  long_value_def = long_value_s21 = 0;
  res_def = sscanf("-123", " %li", &long_value_def);
  res_s21 = s21_sscanf("-123", " %li", &long_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(long_value_def, -123);
  ck_assert_int_eq(long_value_s21, -123);

  long_value_def = long_value_s21 = 0;
  res_def = sscanf("054", "%li", &long_value_def);
  res_s21 = s21_sscanf("054", "%li", &long_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(long_value_def, 054);
  ck_assert_int_eq(long_value_s21, 054);

  long_value_def = long_value_s21 = 0;
  res_def = sscanf("0xFF", "%li", &long_value_def);
  res_s21 = s21_sscanf("0xFF", "%li", &long_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(long_value_def, 0xFF);
  ck_assert_int_eq(long_value_s21, 0xFF);

  long_value_def = long_value_s21 = 0;
  res_def = sscanf("0x4F12F", "%li", &long_value_def);
  res_s21 = s21_sscanf("0x4F12F", "%li", &long_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(long_value_def, 0x4F12F);
  ck_assert_int_eq(long_value_s21, 0x4F12F);

  long_value_def = long_value_s21 = 1;
  res_def = sscanf("0x", "%li", &long_value_def);
  res_s21 = s21_sscanf("0x", "%li", &long_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(long_value_def, 0x0);
  ck_assert_int_eq(long_value_s21, 0x0);

  long_value_def = long_value_s21 = 1;
  res_def = sscanf("0", "%li", &long_value_def);
  res_s21 = s21_sscanf("0", "%li", &long_value_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_int_eq(long_value_def, 0);
  ck_assert_int_eq(long_value_s21, 0);

  // strings
  char str_def[100];
  char str_s21[100];
  const char *text = "Lorem \n  ipsum dolor sit amet";
  res_def = sscanf(text, "%s", str_def);
  res_s21 = s21_sscanf(text, "%s", str_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_str_eq(str_def, "Lorem");
  ck_assert_str_eq(str_s21, "Lorem");

  str_def[0] = str_s21[0] = '\0';
  res_def = sscanf(text, "%20s", str_def);
  res_s21 = s21_sscanf(text, "%20s", str_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_str_eq(str_def, "Lorem");
  ck_assert_str_eq(str_s21, "Lorem");

  str_def[0] = str_s21[0] = '\0';
  res_def = sscanf(text, "%3s", str_def);
  res_s21 = s21_sscanf(text, "%3s", str_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_str_eq(str_def, "Lor");
  ck_assert_str_eq(str_s21, "Lor");

  str_def[0] = str_s21[0] = '\0';
  res_def = sscanf("Lorem", "%s%s", str_def, str_def);
  res_s21 = s21_sscanf("Lorem", "%s%s", str_s21, str_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_str_eq(str_def, "Lorem");
  ck_assert_str_eq(str_s21, "Lorem");

  // ingoring scan result
  int num_def = 0;
  int num_s21 = 0;
  res_def = sscanf("1234 5678 9123", "%*d%d%d", &num_def, &value_def);
  res_s21 = s21_sscanf("1234 5678 9123", "%*d%d%d", &num_s21, &value_s21);
  ck_assert_int_eq(res_def, 2);
  ck_assert_int_eq(res_s21, 2);
  ck_assert_int_eq(num_def, 5678);
  ck_assert_int_eq(num_s21, 5678);
  ck_assert_int_eq(value_def, 9123);
  ck_assert_int_eq(value_s21, 9123);

  num_def = num_s21 = 1;
  res_def = sscanf("1234", "%*d%d", &num_def);
  res_s21 = s21_sscanf("1234", "%*d%d", &num_s21);
  ck_assert_int_eq(res_def, EOF);
  ck_assert_int_eq(res_s21, EOF);
  ck_assert_int_eq(num_def, 1);
  ck_assert_int_eq(num_s21, 1);

  num_def = num_s21 = 1;
  res_def = sscanf("1234 5678 9123", "%d%*d%d", &num_def, &value_def);
  res_s21 = s21_sscanf("1234 5678 9123", "%d%*d%d", &num_s21, &value_s21);
  ck_assert_int_eq(res_def, 2);
  ck_assert_int_eq(res_s21, 2);
  ck_assert_int_eq(num_def, 1234);
  ck_assert_int_eq(num_s21, 1234);
  ck_assert_int_eq(value_def, 9123);
  ck_assert_int_eq(value_s21, 9123);

  res_def = sscanf("1234 5678 9123", "%*d%*d%*d");
  res_s21 = s21_sscanf("1234 5678 9123", "%*d%*d%*d");
  ck_assert_int_eq(res_def, 0);
  ck_assert_int_eq(res_s21, 0);

  str_def[0] = str_s21[0] = '\0';
  res_def = sscanf(text, "%*s%s", str_def);
  res_s21 = s21_sscanf(text, "%*s%s", str_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_str_eq(str_def, "ipsum");
  ck_assert_str_eq(str_s21, "ipsum");

  // symbols and percent
  ch1_def = ch2_def = ch3_def = '\0';
  ch1_s21 = ch2_s21 = ch3_s21 = '\0';
  str_def[0] = str_s21[0] = '\0';
  res_def =
      sscanf("Lorem \n\t  ipsum% dolor sit amet", "%*s\tipsum%%%s", str_def);
  res_s21 = s21_sscanf("Lorem \n\t  ipsum% dolor sit amet", "%*s\tipsum%%%s",
                       str_s21);
  ck_assert_int_eq(res_def, 1);
  ck_assert_int_eq(res_s21, 1);
  ck_assert_str_eq(str_def, "dolor");
  ck_assert_str_eq(str_s21, "dolor");

  // number
  ch1_def = ch2_def = ch3_def = '\0';
  ch1_s21 = ch2_s21 = ch3_s21 = '\0';
  value_def = value_s21 = 0;
  res_def = sscanf(" a   bc", " %c\n%c%n%c", &ch1_def, &ch2_def, &value_def,
                   &ch3_def);
  res_s21 = s21_sscanf(" a   bc", " %c\n%c%n%c", &ch1_s21, &ch2_s21, &value_s21,
                       &ch3_s21);
  ck_assert_int_eq(res_def, 3);
  ck_assert_int_eq(res_s21, 3);
  ck_assert_int_eq(ch1_def, 'a');
  ck_assert_int_eq(ch2_def, 'b');
  ck_assert_int_eq(ch3_def, 'c');
  ck_assert_int_eq(value_def, 6);
  ck_assert_int_eq(ch1_s21, 'a');
  ck_assert_int_eq(ch2_s21, 'b');
  ck_assert_int_eq(ch3_s21, 'c');
  ck_assert_int_eq(value_s21, 6);

  ch1_def = ch2_def = ch3_def = '\0';
  ch1_s21 = ch2_s21 = ch3_s21 = '\0';
  value_def = value_s21 = 0;
  res_def = sscanf(" a   bc", "%n %c\n%c%c", &value_def, &ch1_def, &ch2_def,
                   &ch3_def);
  res_s21 = s21_sscanf(" a   bc", "%n %c\n%c%c", &value_s21, &ch1_s21, &ch2_s21,
                       &ch3_s21);
  ck_assert_int_eq(res_def, 3);
  ck_assert_int_eq(res_s21, 3);
  ck_assert_int_eq(ch1_def, 'a');
  ck_assert_int_eq(ch2_def, 'b');
  ck_assert_int_eq(ch3_def, 'c');
  ck_assert_int_eq(value_def, 0);
  ck_assert_int_eq(ch1_s21, 'a');
  ck_assert_int_eq(ch2_s21, 'b');
  ck_assert_int_eq(ch3_s21, 'c');
  ck_assert_int_eq(value_s21, 0);

  ch1_def = ch2_def = ch3_def = '\0';
  ch1_s21 = ch2_s21 = ch3_s21 = '\0';
  value_def = value_s21 = 0;
  res_def = sscanf("  a   bc", " %n %c\n%c%c", &value_def, &ch1_def, &ch2_def,
                   &ch3_def);
  res_s21 = s21_sscanf("  a   bc", " %n %c\n%c%c", &value_s21, &ch1_s21,
                       &ch2_s21, &ch3_s21);
  ck_assert_int_eq(res_def, 3);
  ck_assert_int_eq(res_s21, 3);
  ck_assert_int_eq(ch1_def, 'a');
  ck_assert_int_eq(ch2_def, 'b');
  ck_assert_int_eq(ch3_def, 'c');
  ck_assert_int_eq(value_def, 2);
  ck_assert_int_eq(ch1_s21, 'a');
  ck_assert_int_eq(ch2_s21, 'b');
  ck_assert_int_eq(ch3_s21, 'c');
  ck_assert_int_eq(value_s21, 2);

  ch1_def = ch2_def = ch3_def = '\0';
  ch1_s21 = ch2_s21 = ch3_s21 = '\0';
  value_def = value_s21 = 0;
  res_def =
      sscanf("a      ", "%c%c%c%n", &ch1_def, &ch2_def, &ch3_def, &value_def);
  res_s21 = s21_sscanf("a      ", "%c%c%c%n", &ch1_s21, &ch2_s21, &ch3_s21,
                       &value_s21);
  ck_assert_int_eq(res_def, 3);
  ck_assert_int_eq(res_s21, 3);
  ck_assert_int_eq(ch1_def, 'a');
  ck_assert_int_eq(ch2_def, ' ');
  ck_assert_int_eq(ch3_def, ' ');
  ck_assert_int_eq(value_def, 3);
  ck_assert_int_eq(ch1_s21, 'a');
  ck_assert_int_eq(ch2_s21, ' ');
  ck_assert_int_eq(ch3_s21, ' ');
  ck_assert_int_eq(value_s21, 3);
}
END_TEST

Suite *s21_sscanf_suite(void) {
  Suite *s = suite_create("s21_sscanf");
  TCase *tc_inner = tcase_create("Inner functions");
  tcase_add_test(tc_inner, test_sscanf_get_length);
  tcase_add_test(tc_inner, test_sscanf_get_modifier);
  tcase_add_test(tc_inner, test_sscanf_get_width);
  tcase_add_test(tc_inner, test_custom_atoi);
  suite_add_tcase(s, tc_inner);

  TCase *tc_formats = tcase_create("Format filling");
  tcase_add_test(tc_formats, test_sscanf_formats);
  suite_add_tcase(s, tc_formats);

  TCase *tc_sscanf = tcase_create("sscanf");
  tcase_add_test(tc_sscanf, test_default_sscanf);
  tcase_add_test(tc_sscanf, test_s21_sscanf);
  suite_add_tcase(s, tc_sscanf);

  return s;
}

static int compare_formats(current_format *format1, current_format *format2) {
  int bitmask = 0;

  if (format1->is_symbol != format2->is_symbol) bitmask += 1;
  bitmask = bitmask << 1;
  if (format1->matching_symbol != format2->matching_symbol) bitmask += 1;
  bitmask = bitmask << 1;

  if (format1->ignore_whitespaces != format2->ignore_whitespaces) bitmask += 1;
  bitmask = bitmask << 1;
  if (format1->ignore != format2->ignore) bitmask += 1;
  bitmask = bitmask << 1;
  if (format1->width != format2->width) bitmask += 1;
  bitmask = bitmask << 1;
  if (format1->length_modifier != format2->length_modifier) bitmask += 1;
  bitmask = bitmask << 1;
  if (format1->type_modifier != format2->type_modifier) bitmask += 1;

  return bitmask;
}