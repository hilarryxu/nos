#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <sys/types.h>

void bzero(void *s, size_t n);

int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);

int ffs(int i);

#endif // _STRINGS_H_
