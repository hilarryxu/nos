#ifndef _STRING_H_
#define _STRING_H_

#include <sys/types.h>

void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

size_t strlen(const char *s);

#endif // _STRING_H_
