#ifndef _LIBK_WCTYPE_H
#define _LIBK_WCTYPE_H

#include <__libk_common.h>

#include <_wint_t.h>
#include <__locale.h>

#ifndef WEOF
#define WEOF ((wint_t) -1)
#endif

typedef int wctype_t;
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

wint_t towctrans(wint_t c, wctrans_t t);
wint_t towupper(wint_t c);
wint_t towlower(wint_t c);

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

wint_t towctrans_(wint_t c, wctrans_t t, locale_t l);
wint_t towupper_(wint_t c, locale_t l);
wint_t towlower_(wint_t c, locale_t l);

wctrans_t wctrans_l(const char *s, locale_t l);
wctype_t wctype_l(const char *s, locale_t l);

#endif
