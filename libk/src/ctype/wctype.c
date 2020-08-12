/**
 * @file wctype.c
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
#include <string.h>
#include <errno.h>

wctype_t wctype(const char *c)
{
    switch (*c)
    {
    case 'a':
        if (!strcmp (c, "alnum"))
        {
            return WC_ALNUM; 
        }
        else if (!strcmp (c, "alpha"))
        {
            return WC_ALPHA;
        }
        break;
    case 'b':
        if (!strcmp (c, "blank"))
        {
            return WC_BLANK;
        }
        break;
    case 'c':
        if (!strcmp (c, "cntrl"))
        {
            return WC_CNTRL;
        }
        break;
    case 'd':
        if (!strcmp (c, "digit"))
        {
            return WC_DIGIT;
        }
        break;
    case 'g':
        if (!strcmp (c, "graph"))
        {
            return WC_GRAPH;
        }
        break;
    case 'l':
        if (!strcmp (c, "lower"))
        {
            return WC_LOWER;
        }
        break;
    case 'p':
        if (!strcmp (c, "print"))
        {
            return WC_PRINT;
        }
        else if (!strcmp (c, "punct"))
        {
            return WC_PUNCT;
        }
        break;
    case 's':
        if (!strcmp (c, "space"))
        {
            return WC_SPACE;
        }
        break;
    case 'u':
        if (!strcmp (c, "upper"))
        {
            return WC_UPPER;
        }
        break;
    case 'x':
        if (!strcmp (c, "xdigit"))
        {
            return WC_XDIGIT;
        }
        break;
    }

    errno = 0;

    return 0;
}
