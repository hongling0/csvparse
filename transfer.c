#include <stdlib.h>
#include <stdio.h>
#include "transfer.h"

double trans_double(const char* str, double def)
{
  char* endptr = NULL;
  double n = strtod(str, &endptr);
  if (*endptr != '\0') {
    fprintf(stderr, "can't convert to double %s\n", str);
    return def;
  }
  return n;
}

long trans_long(const char* str, long def)
{
  char* endptr = NULL;
  long n = strtol(str, &endptr, 0);
  if (*endptr != '\0') {
    fprintf(stderr, "can't convert to long %s\n", str);
    return def;
  }
  return n;
}

