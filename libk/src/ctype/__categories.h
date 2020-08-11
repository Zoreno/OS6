/**
 * @file __categories.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-10
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

#ifndef __CATEGORIES_H
#define __CATEGORIES_H

enum category 
{
  CAT_Cc,
  CAT_Cf,
  CAT_Cs,
  CAT_LC,
  CAT_Ll,
  CAT_Lm,
  CAT_Lo,
  CAT_Lt,
  CAT_Lu,
  CAT_Mc,
  CAT_Me,
  CAT_Mn,
  CAT_Nd,
  CAT_Nl,
  CAT_No,
  CAT_Pc,
  CAT_Pd,
  CAT_Pe,
  CAT_Pf,
  CAT_Pi,
  CAT_Po,
  CAT_Ps,
  CAT_Sc,
  CAT_Sk,
  CAT_Sm,
  CAT_So,
  CAT_Zl,
  CAT_Zp,
  CAT_Zs,
};

enum category __category(wint_t ucs);

wint_t __jp_to_uc(wint_t c, int type);

#endif
