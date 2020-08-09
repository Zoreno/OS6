/**
 * @file pwd_command.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-09
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

#include <simple_cli/commands.h>
#include <simple_cli/simple_cli.h>

#include <stdio.h>

int pwd_command(int argc, const char **argv)
{
    printf("%s\n", simple_cli_get_working_directory());

    return 0;
}