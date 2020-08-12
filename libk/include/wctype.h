#ifndef _LIBK_WCTYPE_H
#define _LIBK_WCTYPE_H

#include <__libk_common.h>

#include <_wint_t.h>
#include <__locale.h>

#ifndef WEOF
#define WEOF ((wint_t) -1)
#endif

/* valid values for wctype_t */
#define WC_ALNUM	1
#define WC_ALPHA	2
#define WC_BLANK	3
#define WC_CNTRL	4
#define WC_DIGIT	5
#define WC_GRAPH	6
#define WC_LOWER	7
#define WC_PRINT	8
#define WC_PUNCT	9
#define WC_SPACE	10
#define WC_UPPER	11
#define WC_XDIGIT	12


typedef int wctype_t;

/* valid values for wctrans_t */
#define WCT_TOLOWER 1
#define WCT_TOUPPER 2

typedef int wctrans_t;

int iswalpha(wint_t c);
int iswalnum(wint_t c);
int iswblank(wint_t c);
int iswcntrl(wint_t c);
int iswctype(wint_t c, wctype_t t);
int iswdigit(wint_t c);
int iswgraph(wint_t c);
int iswlower(wint_t c);
int iswprint(wint_t c);
int iswpunct(wint_t c);
int iswspace(wint_t c);
int iswupper(wint_t c);
int iswxdigit(wint_t c);

wint_t towctrans(wint_t c, wctrans_t t); // TODO
wint_t towupper(wint_t c); // TODO
wint_t towlower(wint_t c); // TODO

wctrans_t wctrans(const char *s);
wctype_t wctype(const char *s);

int iswalpha_l(wint_t c, locale_t l);
int iswalnum_l(wint_t c, locale_t l);
int iswblank_l(wint_t c, locale_t l);
int iswcntrl_l(wint_t c, locale_t l);
int iswctype_l(wint_t c, wctype_t t, locale_t l);
int iswdigit_l(wint_t c, locale_t l);
int iswgraph_l(wint_t c, locale_t l);
int iswlower_l(wint_t c, locale_t l);
int iswprint_l(wint_t c, locale_t l);
int iswpunct_l(wint_t c, locale_t l);
int iswspace_l(wint_t c, locale_t l);
int iswupper_l(wint_t c, locale_t l);
int iswxdigit_l(wint_t c, locale_t l);

wint_t towctrans_l(wint_t c, wctrans_t t, locale_t l); // TODO
wint_t towupper_l(wint_t c, locale_t l); // TODO
wint_t towlower_l(wint_t c, locale_t l); // TODO

wctrans_t wctrans_l(const char *s, locale_t l);
wctype_t wctype_l(const char *s, locale_t l);

#endif
