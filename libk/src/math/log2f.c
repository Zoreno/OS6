/**
 * @file log2f.c
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

float log2f(float a)
{
    union {
        float f;
        int i;
    } vx;

    vx.f = a;

    union {
        float f;
        int i;
    } mx;

    mx.i = (vx.i & 0x007FFFFF) | 0x3F000000;

    a *= 1.1920928955078125e-7f;

    const float A = 124.22551499f;
    const float B = 1.498030302f;
    const float C = 1.72587999f;
    const float D = 0.3520887068f;

    float mxf = mx.f;

    return a - A - B * mxf - C / (D + mxf);
}