#ifndef _LIBK_STRING_H
#define _LIBK_STRING_H

#include <_size_t.h>
#include <_null.h>

void *memchr(const void *str, int c, size_t n);
int memcmp(const void *str1, const void *str2, size_t n);
void *memcpy(void *str1, const void *str2, size_t n);
void *memmove(void *str1, const void *str2, size_t n);
void *memset(void *str, unsigned char c, size_t n);
void *memsetw(void *str, unsigned short c, size_t n);
void *memsetl(void *str, unsigned int c, size_t n);
void *memsetll(void *str, unsigned long long c, size_t n);
void *memccpy(void *dest, const void *src, int c, int n);
void *memmem(const void *haystack_start, size_t haystack_end,
             const void *needle_start, size_t needle_end);
void *mempcpy(void *dest, const void *src, size_t len);
void *memrchr(const void *s, int c_in, size_t n);

char *strcat(char *str1, const char *str2);
char *strncat(char *str1, const char *str2, size_t n);
char *strchr(const char *str1, int c);
int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
char *strcpy(char *str1, const char *str2);
char *strncpy(char *str1, const char *str2, size_t n);
size_t strcspn(const char *str1, const char *reject);
size_t strlen(const char *str);
size_t strnlen(const char *str, size_t maxlen);
char *strpbrk(const char *str1, const char *str2);
char *strrchr(const char *str, int c);
size_t strspn(const char *s, const char *accept);
char *strstr(const char *str1, const char *str2);
char *strtok(char *str, const char *delim);
char *strtok_r(char *s, const char *delim, char **save_ptr);
char *strsep(char **stringp, const char *delim);
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);
int strcasestr(const char *haystack_start, const char *needle_start);
char *strfry(char *string);
char *strdup(const char *str);
char *strndup(const char *str, size_t n);
char *strchrnul(const char *str, int c_in);

int bcmp(const void *s1, const void *s2, size_t n);
int bcopy(const void *src, void *dest, size_t len);
int bzero(const void *s, size_t len);

void *memfrob(void *s, size_t n);

int ffs(int i);
int ffsl(long int i);
int ffsll(long long int i);

void swab(const void *bfrom, void *bto, size_t n);

#endif