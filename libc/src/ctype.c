#include <ctype.h>

int
iscntrl(int c)
{
  return c < ' ';
}

int
isdigit(int c)
{
  return c >= '0' && c <= '9';
}

int
islower(int c)
{
  return c >= 'a' && c <= 'z';
}

int
isprint(int c)
{
  return c >= ' ';
}

int
isspace(int c)
{
  return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
         c == '\v';
}

int
isblank(int c)
{
  return isspace(c);
}

int
isupper(int c)
{
  return c >= 'A' && c <= 'Z';
}

int
isxdigit(int c)
{
  return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

int
isalpha(int c)
{
  return isupper(c) || islower(c);
}

int
isalnum(int c)
{
  return isalpha(c) || isdigit(c);
}

int
isgraph(int c)
{
  return isalnum(c) && c != ' ';
}

int
ispunct(int c)
{
  return isprint(c) && !isalnum(c);
}

char
tolower(int c)
{
  return isupper(c) ? c + ('a' - 'A') : c;
}

char
toupper(int c)
{
  return islower(c) ? c - ('a' - 'A') : c;
}
