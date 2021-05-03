/**
 * @file logging.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2020-12-30
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

#ifndef _LOGGING_H
#define _LOGGING_H

#include <stdarg.h>
#include <stdio.h>

//=============================================================================
// Definitions
//=============================================================================

#define LOG_WARN_FORMAT 0

#if LOG_WARN_FORMAT == 1
#define LOG_WARN_FORMAT_ATTR __attribute__((format(printf, 4, 5)))
#else
#define LOG_WARN_FORMAT_ATTR
#endif

#define LOG_USE_COLOR 1

typedef enum
{
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

//=============================================================================
// Interface functions
//=============================================================================

#define log_trace(...) log_printf(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_printf(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_printf(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_printf(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_printf(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_printf(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Main function for printing out log output.
 *
 * Not intended to be called directly, but rather with the log_<level> macros,
 * which expands the current file and line of the function call.
 *
 * @param level Logging severity of the log printout.
 * @param file File containing the call.
 * @param line Line containing the call.
 * @param fmt printf-like format string.
 * @param ... Rest of the function parameters which will be expanded by the
 * formatting string.
 *
 */
void log_printf(log_level_t level,
                const char *file,
                int line,
                const char *fmt,
                ...) LOG_WARN_FORMAT_ATTR;

/**
 * @brief Get a string representing the log level represented by level.
 *
 * @param level Log level to query.
 * @return Pointer to statically allocated string representing the level.
 */
const char *log_level_string(log_level_t level);

/**
 * @brief Sets the current logging threshold.
 *
 * @param level Current threshold.
 */
void log_set_threshold(log_level_t level);

/**
 * @brief Get the current log level threshold.
 *
 * @return The current logging threshold.
 */
log_level_t log_get_threshold();

/**
 * @brief Initializes the logging unit.
 *
 * @param log_level Log level to use.
 *
 */
void log_init(log_level_t log_level);

#endif

//=============================================================================
// End of file
//=============================================================================
