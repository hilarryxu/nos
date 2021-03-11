#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#define RAND_MAX 32767

__attribute__((__noreturn__)) void abort(void);

int atoi(const char *str);
long atol(const char *str);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
long int strtol(const char *nptr, char **endptr, int base);

int rand(void);

__END_DECLS

#endif /* _STDLIB_H_ */
