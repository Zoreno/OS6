/**
 * @file iswalnum_l.c
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

int iswalnum_l(wint_t c, locale_t l)
{
    enum category cat = __category(c);

    switch(cat)
    {
    case CAT_LC:
    case CAT_Lu:
    case CAT_Ll:
    case CAT_Lt:
    case CAT_Lm:
    case CAT_Lo:
    case CAT_Nl:
    case CAT_Nd:
        return 1;
    default:
        break;
    }
    return 0;
}
