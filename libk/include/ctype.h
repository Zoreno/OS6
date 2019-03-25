#ifndef _LIBK_CTYPE_H
#define _LIBK_CTYPE_H

int isalnum(char c);
int isalpha(char c);
int iscntrl(char c);
int isgraph(char c);
int isdigit(char c);
int islower(char c);
int isprint(char c);
int ispunct(char c);
int isspace(char c);
int isupper(char c);
int isxdigit(char c);

char tolower(char c);
char toupper(char c);

int isascii(char c);
char toascii(char c);

#endif