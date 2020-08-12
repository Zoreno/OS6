/**
 * @file iswctype_l.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-12
 * 
 * @brief 
 * 
 * @copyright Copyright (C) 2020,
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

#include <wctype.h>
#include "__categories.h"

int iswctype_l(wint_t c, wctype_t type, locale_t l)
{
    enum category cat = __category(c);

    switch (desc)
    {
    case WC_ALNUM:
        return iswalnum_l (c, locale);
    case WC_ALPHA:
        return iswalpha_l (c, locale);
    case WC_BLANK:
        return iswblank_l (c, locale);
    case WC_CNTRL:
        return iswcntrl_l (c, locale);
    case WC_DIGIT:
        return iswdigit_l (c, locale);
    case WC_GRAPH:
        return iswgraph_l (c, locale);
    case WC_LOWER:
        return iswlower_l (c, locale);
    case WC_PRINT:
        return iswprint_l (c, locale);
    case WC_PUNCT:
        return iswpunct_l (c, locale);
    case WC_SPACE:
        return iswspace_l (c, locale);
    case WC_UPPER:
        return iswupper_l (c, locale);
    case WC_XDIGIT:
        return iswxdigit_l (c, locale);
    default:
        return 0;
    }

    return 0;
}
