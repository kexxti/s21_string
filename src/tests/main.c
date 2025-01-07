#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include "test_s21_strncat.h"

int main() {
  int number_failed;
  SRunner *sr = srunner_create(s21_strncat_suite());
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? 0 : 1;
}
