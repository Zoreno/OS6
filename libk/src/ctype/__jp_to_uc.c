/**
 * @file __jp_to_uc.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-10
 * 
 * @brief Japanese to unicode conversion
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

#include <string.h>
#include <wctype.h>

#define JP_JIS 1
#define JP_SJIS 2
#define JP_EUCJP 3

wint_t __jp_to_uc(wint_t c, int type)
{
    int index;
    int adj;
    unsigned char byte1;
    unsigned char byte2;
    wint_t ret;

    switch (type)
    {
    case JP_JIS:
        byte1 = (c >> 8) + 0x80;
        byte2 = (c & 0xFF) + 0x80;
        break;

    case JP_SJIS:
        byte1 = (c >> 8);
        byte2 = (c & 0xFF);
        break;
    
    case JP_EUCJP:
        byte1 = (c >> 8);
        byte2 = (c & 0xFF);

        if (byte2 <= 0x9e)
        {
            adj = 0xa1 - 0x22;
            byte2 = (byte2 - 31) + 0xa1;
        }
        else
        {
            adj = 0xa1 - 0x21;
            byte2 = (byte2 - 126) + 0xa1;
        }

        if (byte1 <= 0x9f)
        {
            byte1 = ((byte1 - 112) << 1) + adj;
        }
        else
        {
            byte1 = ((byte1 - 176) << 1) + adj;
        }
        
        break; 

    default:
        break;
    }

    return WEOF;
}
