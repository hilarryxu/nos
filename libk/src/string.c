#include <string.h>

#include <ctype.h>

int
memcmp(const void *aptr, const void *bptr, size_t size)
{
  const unsigned char *a = (const unsigned char *)aptr;
  const unsigned char *b = (const unsigned char *)bptr;

  for (size_t i = 0; i < size; i++) {
    if (a[i] < b[i])
      return -1;
    else if (b[i] < a[i])
      return 1;
  }
  return 0;
}

void *
memcpy(void *restrict dstptr, const void *restrict srcptr, size_t size)
{
  unsigned char *dst = (unsigned char *)dstptr;
  const unsigned char *src = (const unsigned char *)srcptr;

  for (size_t i = 0; i < size; i++)
    dst[i] = src[i];
  return dstptr;
}

void *
memmove(void *dstptr, const void *srcptr, size_t size)
{
  unsigned char *dst = (unsigned char *)dstptr;
  const unsigned char *src = (const unsigned char *)srcptr;

  if (dst < src) {
    for (size_t i = 0; i < size; i++)
      dst[i] = src[i];
  } else {
    for (size_t i = size; i != 0; i--)
      dst[i - 1] = src[i - 1];
  }
  return dstptr;
}

void *
memset(void *bufptr, int value, size_t size)
{
  unsigned char *buf = (unsigned char *)bufptr;

  for (size_t i = 0; i < size; i++)
    buf[i] = (unsigned char)value;
  return bufptr;
}

void *
memchr(const void *ptr, int c, size_t n)
{
  unsigned char ch = (unsigned char)c;
  const unsigned char *p = ptr;

  while (n-- > 0) {
    if (*p == ch)
      return (void *)p;
    p += 1;
  }

  return NULL;
}

size_t
strlen(const char *s)
{
  size_t len = 0;

  while (s[len])
    len++;
  return len;
}

int
strcmp(const char *s1, const char *s2)
{
  while (*s1 != 0 && *s2 != 0) {
    if (*s1 != *s2)
      break;
    s1 += 1;
    s2 += 1;
  }

  return *s1 - *s2;
}

int
strncmp(const char *s1, const char *s2, size_t n)
{
  if (n < 1)
    return 0;

  while (*s1 != 0 && *s2 != 0 && --n > 0) {
    if (*s1 != *s2)
      break;
    s1 += 1;
    s2 += 1;
  }

  return *s1 - *s2;
}

char *
strcpy(char *dst, const char *src)
{
  char *ptr = dst;

  do {
    *ptr++ = *src;
  } while (*src++ != 0);

  return dst;
}

char *
strcat(char *dst, const char *src)
{
  int p;

  p = strlen(dst);
  strcpy(&dst[p], src);

  return dst;
}

char *
strncpy(char *dst, const char *src, size_t n)
{
  char *ret = dst;

  /* Copy string */
  while (*src != 0 && n > 0) {
    *dst++ = *src++;
    n -= 1;
  }

  /* strncpy always clears the rest of destination string... */
  while (n > 0) {
    *dst++ = 0;
    n -= 1;
  }

  return ret;
}

char *
strchr(const char *s, int c)
{
  char cb = c;

  while (*s != 0) {
    if (*s == cb) {
      return (char *)s;
    }
    s += 1;
  }

  return NULL;
}

char *
strstr(const char *hay, const char *needle)
{
  char *pos;
  int hlen, nlen;

  if (hay == NULL || needle == NULL)
    return NULL;

  hlen = strlen(hay);
  nlen = strlen(needle);
  if (nlen < 1)
    return (char *)hay;

  for (pos = (char *)hay; pos < hay + hlen; pos++) {
    if (strncmp(pos, needle, nlen) == 0) {
      return pos;
    }
  }

  return NULL;
}

int
strcasecmp(const char *s1, const char *s2)
{
  while (*s1 != 0 && *s2 != 0) {
    if (toupper(*s1) != toupper(*s2))
      break;
    ++s1;
    ++s2;
  }

  return *s1 - *s2;
}

int
strncasecmp(const char *s1, const char *s2, size_t n)
{
  if (n < 1)
    return 0;

  while (*s1 != 0 && *s2 != 0 && --n > 0) {
    if (toupper(*s1) != toupper(*s2))
      break;
    ++s1;
    ++s2;
  }

  return toupper(*s1) - toupper(*s2);
}
