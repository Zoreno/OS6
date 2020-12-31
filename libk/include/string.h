/**
 * @file string.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-25
 * 
 * @brief C standard library string and memory functions
 * 
 * @copyright Copyright (C) 2019,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 * 
 */

#ifndef _LIBK_STRING_H
#define _LIBK_STRING_H

#include <__libk_common.h>

#include <_null.h>
#include <_size_t.h>

int bcmp(const void *s1, const void *s2, size_t n);  // Done
void bcopy(const void *src, void *dest, size_t len); // Done
void bzero(void *s, size_t len);

int ffs(int i);             // Done
int ffsl(long int i);       // Done
int ffsll(long long int i); // Done

int fls(int i);             // Done
int flsl(long int i);       // Done
int flsll(long long int i); // Done

char *index(const char *s, int c); // Done

void *memccpy(void *dest, const void *src, int c, size_t n); // Done
void *memchr(const void *str, int c, size_t n);              // Done
int memcmp(const void *str1, const void *str2, size_t n);    // Done
void *memcpy(void *str1, const void *str2, size_t n);        // Done
void *memcpyl(void *str1, const void *str2, size_t n);       // Done
void *memcpyw(void *str1, const void *str2, size_t n);       // Done
void *memfrob(void *s, size_t n);                            // Done
void *memmem(const void *haystack_start, size_t haystack_len,
             const void *needle_start, size_t needle_len); // Done
void *memmove(void *str1, const void *str2, size_t n);     // Done
void *mempcpy(void *dest, const void *src, size_t len);    // Done
void *memrchr(const void *s, int c, size_t n);             // Done
void *memset(void *str, unsigned char c, size_t n);        // Done
void *memsetl(void *str, unsigned int c, size_t n);        // Done
void *memsetll(void *str, unsigned long long c, size_t n); // Done
void *memsetw(void *str, unsigned short c, size_t n);      // Done

char *rindex(const char *s, int c); // Done

char *strcat(char *str1, const char *str2);                  // Done
char *strchr(const char *str1, int c);                       // Done
int strcmp(const char *str1, const char *str2);              // Done
char *strcpy(char *str1, const char *str2);                  // Done
size_t strcspn(const char *str1, const char *reject);        // Done
char *strdup(const char *str);                               // Done
size_t strlen(const char *str);                              // Done
char *strncat(char *str1, const char *str2, size_t n);       // Done
int strncmp(const char *str1, const char *str2, size_t n);   // Done
char *strncpy(char *str1, const char *str2, size_t n);       // Done
size_t strnlen(const char *str, size_t maxlen);              // Done
char *strpbrk(const char *str1, const char *str2);           // Done
char *strrchr(const char *str, int c);                       // Done
size_t strspn(const char *s, const char *accept);            // Done
char *strstr(const char *str1, const char *str2);            // Done
char *strtok_r(char *s, const char *delim, char **save_ptr); // Done
char *strtok(char *str, const char *delim);                  // Done

// Missing functions (Move to above in alphabetical order when implemented)

char *strsep(char **stringp, const char *delim);                      // TODO
int strcasecmp(const char *s1, const char *s2);                       // TODO
int strncasecmp(const char *s1, const char *s2, size_t n);            // TODO
int strcasestr(const char *haystack_start, const char *needle_start); // TODO
char *strfry(char *string);                                           // TODO
char *strndup(const char *str, size_t n);                             // TODO
char *strchrnul(const char *str, int c_in);                           // TODO
void swab(const void *bfrom, void *bto, size_t n);                    // TODO

#endif

//=============================================================================
// End of file
//=============================================================================