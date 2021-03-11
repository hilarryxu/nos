#include <stdio.h>
#include <stdlib.h>

#if defined(_KERNEL_)
#include <nos/nos.h>
#endif

__attribute__((__noreturn__)) void
abort(void)
{
#if defined(_KERNEL_)
  printk("\nkernel: panic: abort()\n");
#endif

  while (1) {
  }
  __builtin_unreachable();
}

long int
strtol(const char *S, char **PTR, int BASE)
{
  long rval = 0;
  short int negative = 0;
  short int digit;
  // *PTR is S, unless PTR is NULL, in which case i override it with my own ptr
  char *ptr;
  if (PTR == NULL) {
    // override
    PTR = &ptr;
  }
  // i use PTR to advance through the string
  *PTR = (char *)S;
  // check if BASE is ok
  if ((BASE < 0) || BASE > 36) {
    return 0;
  }
  // ignore white space at beginning of S
  while ((**PTR == ' ') || (**PTR == '\t') || (**PTR == '\n') ||
         (**PTR == '\r')) {
    (*PTR)++;
  }
  // check if S starts with "-" in which case the return value is negative
  if (**PTR == '-') {
    negative = 1;
    (*PTR)++;
  }
  // if BASE is 0... determine the base from the first chars...
  if (BASE == 0) {
    // if S starts with "0x", BASE = 16, else 10
    if ((**PTR == '0') && (*((*PTR) + 1) == 'x')) {
      BASE = 16;
    } else {
      BASE = 10;
    }
  }
  if (BASE == 16) {
    // S may start with "0x"
    if ((**PTR == '0') && (*((*PTR) + 1) == 'x')) {
      (*PTR)++;
      (*PTR)++;
    }
  }
  // until end of string
  while (**PTR) {
    if (((**PTR) >= '0') && ((**PTR) <= '9')) {
      // digit (0..9)
      digit = **PTR - '0';
    } else if (((**PTR) >= 'a') && ((**PTR) <= 'z')) {
      // alphanumeric digit lowercase(a (10) .. z (35) )
      digit = (**PTR - 'a') + 10;
    } else if (((**PTR) >= 'A') && ((**PTR) <= 'Z')) {
      // alphanumeric digit uppercase(a (10) .. z (35) )
      digit = (**PTR - 'A') + 10;
    } else {
      // end of parseable number reached...
      break;
    }
    if (digit < BASE) {
      rval = (rval * BASE) + digit;
    } else {
      // digit found, but its too big for current base
      // end of parseable number reached...
      break;
    }
    // next...
    (*PTR)++;
  }
  if (negative) {
    return rval * -1;
  }
  // else
  return rval;
}

unsigned long int
strtoul(const char *S, char **PTR, int BASE)
{
  unsigned long rval = 0;
  short int digit;
  // *PTR is S, unless PTR is NULL, in which case i override it with my own ptr
  char *ptr;
  if (PTR == NULL) {
    // override
    PTR = &ptr;
  }
  // i use PTR to advance through the string
  *PTR = (char *)S;
  // check if BASE is ok
  if ((BASE < 0) || BASE > 36) {
    return 0;
  }
  // ignore white space at beginning of S
  while ((**PTR == ' ') || (**PTR == '\t') || (**PTR == '\n') ||
         (**PTR == '\r')) {
    (*PTR)++;
  }
  // if BASE is 0... determine the base from the first chars...
  if (BASE == 0) {
    // if S starts with "0x", BASE = 16, else 10
    if ((**PTR == '0') && (*((*PTR) + 1) == 'x')) {
      BASE = 16;
    } else {
      BASE = 10;
    }
  }
  if (BASE == 16) {
    // S may start with "0x"
    if ((**PTR == '0') && (*((*PTR) + 1) == 'x')) {
      (*PTR)++;
      (*PTR)++;
    }
  }
  // until end of string
  while (**PTR) {
    if (((**PTR) >= '0') && ((**PTR) <= '9')) {
      // digit (0..9)
      digit = **PTR - '0';
    } else if (((**PTR) >= 'a') && ((**PTR) <= 'z')) {
      // alphanumeric digit lowercase(a (10) .. z (35) )
      digit = (**PTR - 'a') + 10;
    } else if (((**PTR) >= 'A') && ((**PTR) <= 'Z')) {
      // alphanumeric digit uppercase(a (10) .. z (35) )
      digit = (**PTR - 'A') + 10;
    } else {
      // end of parseable number reached...
      break;
    }
    if (digit < BASE) {
      rval = (rval * BASE) + digit;
    } else {
      // digit found, but its too big for current base
      // end of parseable number reached...
      break;
    }
    // next...
    (*PTR)++;
  }
  // done
  return rval;
}

int
atoi(const char *str)
{
  return strtol(str, NULL, 10);
}

long
atol(const char *str)
{
  return strtol(str, NULL, 10);
}

static unsigned long _rand = 1;

int
rand(void)
{
  _rand = _rand * 25364735 + 34563;

  return ((unsigned int)(_rand >> 16) & RAND_MAX);
}
