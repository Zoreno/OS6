/**
 * @file atanf.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-26
 * 
 * @brief 
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

#include <math.h>

float atanf(float a)
{
    int gt1 = a >= 1.f;

    if (gt1)
    {
        a = rcpf(a);
    }

    int lt1 = a <= -1.f;

    if (lt1)
    {
        a = rcpf(a);
    }

    const float A0 = 1.0f;
    const float A1 = 0.33288950512027f;
    const float A2 = -0.08467922817644f;
    const float A3 = 0.03252232640125f;
    const float A4 = -0.00749305860992f;

    const float a2 = a * a;

    const float f = A0 + (A1 + (A2 + (A3 + A4 * a2) * a2) * a2) * a2;

    float res = a / f;

    if (gt1)
    {
        res = M_PI_2 - res;
    }

    if (lt1)
    {
        res = -M_PI_2 - res;
    }

    return res;
}