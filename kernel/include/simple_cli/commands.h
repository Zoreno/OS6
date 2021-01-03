/**
 * @file commands.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-08-09
 * 
 * @brief Simple CLI commands
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

#ifndef _COMMANDS_H
#define _COMMANDS_H

int test_command(int argc, const char** argv);
int cd_command(int argc, const char** argv);
int ls_command(int argc, const char** argv);
int mkdir_command(int argc, const char** argv);
int pwd_command(int argc, const char** argv);
int rm_command(int argc, const char** argv);
int exit_command(int argc, const char** argv);
int time_command(int argc, const char** argv);

#endif

//=============================================================================
// End of file
//=============================================================================