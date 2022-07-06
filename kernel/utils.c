#include "utils.h"

// copy set of bytes by the size(2) from the source(0) to destination(1)
void memncpy(char* source, char* dest, int n) {
  for (int i = 0; i < n; i++) *(dest + i) = *(source + i);
}

void int_to_ascii(int n, char str[]) {
  int i, sign;
  if ((sign = n) < 0) n = -n;
  i = 0;
  do {
    str[i++] = n % 10 + '0';
  } while ((n /= 10) > 0);

  if (sign < 0) str[i++] = '-';
  str[i] = '\0';

  /* TODO: implement "reverse" */
}
