#include <string.h>
#include <stdint.h>

void *
memset(void *s, int c, size_t n)
{
  size_t i;
  uint8_t *byte_dest = (uint8_t *)s;

  for (i = 0; i < n; i++)
    byte_dest[i] = c;
  return s;
}

void *
memmove(void *dest, const void *src, size_t n)
{
  const char *s;
  char *d;

  s = src;
  d = dest;
  if (s < d && s + n > d) {
    s += n;
    d += n;
    while (n-- > 0)
      *--d = *--s;
  } else
    while (n-- > 0)
      *d++ = *s++;

  return dest;
}

void *
memcpy(void *dest, const void *src, size_t n)
{
  return memmove(dest, src, n);
}

int
memcmp(const void *v1, const void *v2, size_t n)
{
  const uint8_t *s1, *s2;

  s1 = v1;
  s2 = v2;
  while (n-- > 0) {
    if (*s1 != *s2)
      return *s1 - *s2;
    s1++, s2++;
  }

  return 0;
}

size_t
strlen(const char *s)
{
  size_t i;

  for (i = 0; s[i]; i++)
    ;
  return i;
}
