#ifndef _CTYPE_H_
#define _CTYPE_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#define _toupper(c) toupper(c)
#define _tolower(c) tolower(c)

int iscntrl(int c);
int isdigit(int c);
int islower(int c);
int isprint(int c);
int isspace(int c);
int isblank(int c);
int isupper(int c);
int isxdigit(int c);
int isalpha(int c);
int isalnum(int c);
int isgraph(int c);
int ispunct(int c);
char tolower(int c);
char toupper(int c);

__END_DECLS

#endif /* !_CTYPE_H_ */
