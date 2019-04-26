/**
 * @file pow2f.c
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

#include <fenv.h>

float pow2f(float a)
{
    int old_round = fegetround();

    fesetround(FE_DOWNWARD);

    float offset = 0.f;

    if (a < 0)
    {
        offset = 1.f;
    }

    float clipp = a < -126.f ? -126.f : a;
    int w = (float)clipp;

    float z = clipp - w + offset;

    float A = 121.2740575f;
    float B = 27.7280233f;
    float C = 4.84252568f;
    float D = 1.49012907f;

    float c = (float)(1 << 23) * (clipp + A + B / (C + z) - D * z);

    fesetround(old_round);

    return c;
}