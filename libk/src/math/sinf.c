/**
 * @file sinf.c
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

float sinf(float a)
{
    const float B = 4.f / M_PI;
    const float C = -4.f / (M_PI * M_PI);
    const float P = 0.255f;

    float m = fmodf(a + M_2_PI, 2 * M_2_PI) - M_2_PI;

    float m_abs = fabsf(m);

    float m_y = (m_abs * C + B) * a;
    m_y += (fabsf(m_y) * m_y - m_y) * P;

    return m_y;
}