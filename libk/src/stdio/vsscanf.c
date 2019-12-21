/** \addtogroup libk 
 *  @{
 */

/**
 * @file vsscanf.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-12-14
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

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

//==============================================================================
// Implementation types
//==============================================================================

/**
 * @brief Enum describing all possible length specifiers
 * 
 * 
 */
typedef enum _length_specifier_t
{
    /**
     * @brief Indicates that no specifier was passed
     * 
     * 
     */
    LENGTH_SPECIFIER_NONE,

    /**
     * @brief Indicates that 'hh' was passed
     * 
     * 
     */
    LENGTH_SPECIFIER_hh,

    /**
     * @brief Indicates that 'h' was passed
     * 
     * 
     */
    LENGTH_SPECIFIER_h,

    /**
     * @brief Indicates that 'll' was passed
     * 
     * 
     */
    LENGTH_SPECIFIER_ll,

    /**
     * @brief Indicates that 'l' was passed
     * 
     * 
     */
    LENGTH_SPECIFIER_l,

    /**
     * @brief Indicates that 'j' was passed
     * 
     * 
     */
    LENGTH_SPECIFIER_j,

    /**
     * @brief Indicates that 'z' was passed
     * 
     * 
     */
    LENGTH_SPECIFIER_z,

    /**
     * @brief Indicates that 't' was passed
     * 
     * 
     */
    LENGTH_SPECIFIER_t,

    /**
     * @brief Indicates that 'L' was passed
     * 
     * 
     */
    LENGTH_SPECIFIER_L,

    /**
     * @brief The number of length specifiers.
     * 
     * 
     */
    LENGTH_SPECIFIER_COUNT,
} length_specifier_t;

/**
 * @brief Enum describing all possible type specifiers
 * 
 * 
 */
typedef enum _specifier_t
{
    /**
     * @brief Integer specifier.
     * 
     * 
     */
    SPECIFIER_i,

    /**
     * @brief Decimal integer specifier.
     * 
     * 
     */
    SPECIFIER_d,

    /**
     * @brief Unsinged integer specifier.
     * 
     * 
     */
    SPECIFIER_u,

    /**
     * @brief Octal integer specifier.
     * 
     * 
     */
    SPECIFIER_o,

    /**
     * @brief Hexadecimal integer specifier.
     * 
     * 
     */
    SPECIFIER_x,

    /**
     * @brief Floating point specifier.
     * 
     * 
     */
    SPECIFIER_f,

    /**
     * @brief Floating point specifier with exponent.
     * 
     * 
     */
    SPECIFIER_e,

    /**
     * @brief Floating point specifier.
     * 
     * 
     */
    SPECIFIER_g,

    /**
     * @brief Floating point specifier.
     * 
     * 
     */
    SPECIFIER_a,

    /**
     * @brief Single character specifier.
     * 
     * 
     */
    SPECIFIER_c,

    /**
     * @brief Character string specifier.
     * 
     * 
     */
    SPECIFIER_s,

    /**
     * @brief Pointer specifier.
     * 
     * 
     */
    SPECIFIER_p,

    /**
     * @brief Scanset specifier.
     * 
     * 
     */
    SPECIFIER_scanset,

    /**
     * @brief Negated scanset specifier.
     * 
     * 
     */
    SPECIFIER_neg_scanset,

    /**
     * @brief Output number of consumed characters.
     * 
     * 
     */
    SPECIFIER_n,

    /**
     * @brief Output the literal '%'.
     * 
     * 
     */
    SPECIFIER_percent,

    /**
     * @brief Number of specifiers.
     * 
     * 
     */
    SPECIFIER_COUNT,
} specifier_t;

/**
 * @brief Compound of all flags for a given type specifier in the format string.
 * 
 * 
 */
typedef struct __scanf_flags
{
    /**
     * @brief Maximum number of characters to read
     * 
     * 
     */
    size_t read_max;

    /**
     * @brief Suppress storing match into a variable
     * 
     * 
     */
    int do_not_store;

    /**
     * @brief Determines how large the variable passed on the stack should be.
     * 
     * Valid length specifiers are specified in enum @length_specifier_t.
     */
    length_specifier_t length_specifier;

    /**
     * @brief Determines how the string should be interpreted.
     * 
     * Valid specifiers are specified in enum @specifier_t.
     */
    specifier_t specifier;

    /**
     * @brief Scanset used in the case of a [characters] or [^characters] format specifier.
     * 
     * One bit for each ascii character
     */
    char scanset[256 / 8];
} _scanf_flags;

//==============================================================================
// Implementation function forward declarations
//==============================================================================

/**
 * @brief Consumes a character, storing it in context and increments format_ptr.
 * 
 * @param context Pointer to character variable to store the value.
 * @param format_ptr Pointer to format.
 * 
 */
static void _consume_char(char *context, const char **format_ptr);

/**
 * @brief Skips past any leading whitespace in the string.
 * 
 * @param string Pointer to string 
 * 
 */
static void _skip_whitespace(const char **string);

/**
 * @brief Determines if a character should be considered whitespace.
 * 
 * @param c Character to classify.
 * 
 * @return Non-zero if character is whitespace.
 */
static int _iswhitespace(char c);

/**
 * @brief Determines if character is a valid octal number.
 * 
 * @param c Character to classify.
 * 
 * @return Non-zero if character is octal.
 */
static int _isoctal(char c);

/**
 * @brief Determines if a character is a valid specifier.
 * 
 * @param c Character to classify.
 * 
 * @return Non-zero if character is a specifier.
 */
static int _isspecifier(char c);

/**
 * @brief Tries to parse a format specifier.
 * 
 * @param current_char Pointer to current character.
 * @param format Pointer to format string.
 * @param flags Pointer to struct to store the flags.
 * 
 * @return Non-zero if successful.
 */
static int _parse_format(char *current_char, const char **format, _scanf_flags *flags);

/**
 * @brief Tries to parse a scanset
 * 
 * @param current_char Pointer to current character.
 * @param format Pointer to format string.
 * @param flags Pointer to struct to store the flags.
 * 
 * @return Non-zero if successful 
 */
static int _parse_scanset(char *current_char, const char **format, _scanf_flags *flags);

//==============================================================================
// Implementation private variables
//==============================================================================

/**
 * @brief String containing all valid specifiers.
 * 
 * 
 */
static const char *_specifiers = "diuoxfegacspn[";

//==============================================================================
// Implementation function definitions
//==============================================================================

static void _consume_char(char *context, const char **format_ptr)
{
    *context = **format_ptr;
    (*format_ptr)++;
}

static void _skip_whitespace(const char **string)
{
    while (**string && _iswhitespace(**string))
    {
        (*string)++;
    }
}

static int _iswhitespace(char c)
{
    // isspace includes space and tab and newline
    return isspace(c) || c == '\n' || c == '\r';
}

static int _isoctal(char c)
{
    return c >= '0' && c < '8';
}

static int _isspecifier(char c)
{
    size_t numSpecifiers = strlen(_specifiers);

    for (size_t i = 0; i < numSpecifiers; ++i)
    {
        if (c == _specifiers[i])
        {
            return 1;
        }
    }

    return 0;
}

static int _parse_format(char *current_char, const char **format, _scanf_flags *flags)
{
    //==========================================================================
    // Do not store flag
    //==========================================================================

    if (*current_char == '*')
    {
        _consume_char(current_char, format);

        flags->do_not_store = 1;
    }

    //==========================================================================
    // Max width limit
    //==========================================================================

    // Handle length specifier: i.e %10s
    if (isdigit(*current_char))
    {
        // TODO: This should be enough
        char number[65] = {0};
        int current_pos = 0;

        while (isdigit(*current_char))
        {
            number[current_pos++] = *current_char;
            _consume_char(current_char, format);
        }
        number[current_pos] = 0;

        flags->read_max = strtoull(number, NULL, 10);
    }

    //==========================================================================
    // Handle length specifier
    //==========================================================================

    if (*current_char == 'l')
    {
        _consume_char(current_char, format);

        if (*current_char == 'l')
        {
            _consume_char(current_char, format);

            flags->length_specifier = LENGTH_SPECIFIER_ll;
        }
        else
        {
            flags->length_specifier = LENGTH_SPECIFIER_l;
        }
    }
    else if (*current_char == 'h')
    {
        _consume_char(current_char, format);

        if (*current_char == 'h')
        {
            _consume_char(current_char, format);

            flags->length_specifier = LENGTH_SPECIFIER_hh;
        }
        else
        {
            flags->length_specifier = LENGTH_SPECIFIER_h;
        }
    }
    else if (*current_char == 'j')
    {
        _consume_char(current_char, format);

        flags->length_specifier = LENGTH_SPECIFIER_j;
    }
    else if (*current_char == 'z')
    {
        _consume_char(current_char, format);

        flags->length_specifier = LENGTH_SPECIFIER_z;
    }
    else if (*current_char == 't')
    {
        _consume_char(current_char, format);

        flags->length_specifier = LENGTH_SPECIFIER_t;
    }
    else if (*current_char == 'L')
    {
        _consume_char(current_char, format);

        flags->length_specifier = LENGTH_SPECIFIER_L;
    }
    else
    {
        flags->length_specifier = LENGTH_SPECIFIER_NONE;
    }

    //==========================================================================
    // Handle format specifier
    //==========================================================================

    if (*current_char == 'i')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_i;
    }
    else if (*current_char == 'd')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_d;
    }
    else if (*current_char == 'u')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_u;
    }
    else if (*current_char == 'o')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_o;
    }
    else if (*current_char == 'x')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_x;
    }
    else if (*current_char == 'f')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_f;
    }
    else if (*current_char == 'e')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_e;
    }
    else if (*current_char == 'g')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_g;
    }
    else if (*current_char == 'a')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_a;
    }
    else if (*current_char == 'c')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_c;
    }
    else if (*current_char == 's')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_s;
    }
    else if (*current_char == 'p')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_p;
    }
    else if (*current_char == '[')
    {
        _consume_char(current_char, format);

        _parse_scanset(current_char, format, flags);
    }
    else if (*current_char == 'n')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_n;
    }
    else if (*current_char == 'n')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_n;
    }
    else
    {
        return 0;
    }

    //==========================================================================
    // Done
    //==========================================================================

    return 1;
}

static int _parse_scanset(char *current_char, const char **format, _scanf_flags *flags)
{
    _consume_char(current_char, format);

    if (*current_char == '^')
    {
        _consume_char(current_char, format);

        flags->specifier = SPECIFIER_neg_scanset;
    }
    else
    {
        flags->specifier = SPECIFIER_scanset;
    }

    while (*current_char != ']')
    {
        char c = *current_char;

        // Unexpected end of string
        if (!c)
        {
            return 0;
        }

        // Ranges a-z are a non-standard extension and not supported for now.

        flags->scanset[c / 8] |= (1 << (c % 8));

        _consume_char(current_char, format);
    }

    // Make sure to also consume the ']' char
    _consume_char(current_char, format);

    return 1;
}

//==============================================================================
// Interface functions
//==============================================================================

int vsscanf(const char *str, const char *format, va_list ap)
{
    int count = 0;

    char current_char;

    _consume_char(&current_char, &format);

    while (current_char)
    {
        printf("current char: %c\n", current_char);
        printf("Str: [%s]\n", str);
        printf("Format: [%s]\n", format);

        // Read past any whitespace in the input buffer
        if (_iswhitespace(current_char))
        {
            _skip_whitespace(&str);
        }

        // Handle special format
        else if (current_char == '%')
        {
            // Consume the percent sign from the
            _consume_char(&current_char, &format);

            _scanf_flags flags;

            flags.do_not_store = 0;
            flags.read_max = SIZE_MAX;

            if (!_parse_format(&current_char, &format, &flags))
            {
                return count;
            }

#if 0

            // Handle integer numbers
            if (current_char == 'd')
            {
                // Always read as long long int
                long long int i = 0;
                int sign = 1;
                unsigned long long int read_count = 0;
                _skip_whitespace(&str);

                if (*str == '-')
                {
                    sign = -1;
                    str++;
                }
                else if (*str == '+')
                {
                    str++;
                }

                while (read_count < _read_max && *str && isdigit(*str))
                {
                    i = i * 10 + (*str - '0');
                    ++str;
                    ++read_count;
                }

                if (!_do_not_store)
                {
                    if (_size == 2)
                    {
                        long long int *out = (long long int *)va_arg(ap, long long int *);

                        *out = (long long int)i * sign;
                    }

                    if (_size == 1)
                    {
                        long int *out = (long int *)va_arg(ap, long int *);

                        *out = (long int)i * sign;
                    }
                    else
                    {
                        int *out = (int *)va_arg(ap, int *);

                        *out = (int)i * sign;
                    }

                    ++count;
                }
            }

            // Handle unsigned integer numbers
            else if (current_char == 'u')
            {
                // Always read as long long int
                unsigned long long int i = 0;
                unsigned long long int read_count = 0;
                _skip_whitespace(&str);

                while (read_count < _read_max && *str && isdigit(*str))
                {
                    i = i * 10 + (*str - '0');
                    ++str;
                    ++read_count;
                }

                if (!_do_not_store)
                {
                    if (_size == 2)
                    {
                        unsigned long long int *out = (unsigned long long int *)va_arg(ap, unsigned long long int *);

                        *out = (unsigned long long int)i;
                    }

                    if (_size == 1)
                    {
                        unsigned long int *out = (unsigned long int *)va_arg(ap, unsigned long int *);

                        *out = (unsigned long int)i;
                    }
                    else
                    {
                        unsigned int *out = (unsigned int *)va_arg(ap, unsigned int *);

                        *out = (unsigned int)i;
                    }

                    ++count;
                }
            }

            // Handle integer numbers
            else if (current_char == 'i')
            {
                // Always read as long long int
                long long int i = 0;
                int sign = 1;
                int base = 10;
                unsigned long long int read_count = 0;
                _skip_whitespace(&str);

                if (*str == '-')
                {
                    sign = -1;
                    ++str;
                }
                else if (*str == '+')
                {
                    ++str;
                }

                if (*str == '0')
                {
                    ++str;

                    // Hexadecimal number
                    if (*str == 'x')
                    {
                        ++str;
                        base = 16;
                    }

                    // Octal number
                    else
                    {
                        base = 8;
                    }
                }

                if (base == 8)
                {
                    while (read_count < _read_max && *str && _isoctal(*str))
                    {
                        i = i * base + (*str - '0');
                        ++str;
                        ++read_count;
                    }
                }
                else if (base == 16)
                {
                    while (read_count < _read_max && *str && isxdigit(*str))
                    {
                        int value;

                        if (*str >= 'a')
                        {
                            value = *str - 'a';
                        }
                        else if (*str >= 'A')
                        {
                            value = *str - 'A';
                        }
                        else
                        {
                            value = *str - '0';
                        }
                        i = i * base + value;
                        ++str;
                        ++read_count;
                    }
                }
                else
                {
                    while (read_count < _read_max && *str && isdigit(*str))
                    {
                        i = i * base + (*str - '0');
                        ++str;
                        ++read_count;
                    }
                }

                if (!_do_not_store)
                {
                    if (_size == 2)
                    {
                        long long int *out = (long long int *)va_arg(ap, long long int *);

                        *out = (long long int)i * sign;
                    }

                    if (_size == 1)
                    {
                        long int *out = (long int *)va_arg(ap, long int *);

                        *out = (long int)i * sign;
                    }
                    else
                    {
                        int *out = (int *)va_arg(ap, int *);

                        *out = (int)i * sign;
                    }

                    ++count;
                }
            }

            else
            {
                printf("Error matching format 1\n");
            }

#endif
        }

        // Handle exact character
        else
        {
            if (*str == current_char)
            {
                str++;
            }
            else
            {
                printf("Error matching format 2\n");
                break;
            }
        }

        _consume_char(&current_char, &format);
    }

    return count;
}

/** @}*/

//==============================================================================
// End of file
//==============================================================================