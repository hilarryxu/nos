#include <strings.h>
#include <string.h>

void
bzero(void *s, size_t n)
{
  memset(s, '\0', n);
}

int
strcasecmp(const char *s1, const char *s2)
{
  size_t i;

  for (i = 0; (s1[i] != 0 && s2[i] != 0); i++) {
    if (s1[i] != s2[i])
      return s1[i] - s2[i];
  }
  return 0;
}

int
strncasecmp(const char *s1, const char *s2, size_t n)
{
  size_t i;

  for (i = 0; (s1[i] != 0 && s2[i] != 0 && i < n); i++) {
    if (s1[i] != s2[i])
      return s1[i] - s2[i];
  }
  return 0;
}

int
ffs(int x)
{
  size_t i;

  for (i = 0; i < sizeof(int) * 8; i++) {
    if (x & (1 << i))
      return i + 1;
  }
  return 0;
}
