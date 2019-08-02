/**
 * @file json.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-07-09
 * 
 * @brief JSON parser
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

#ifndef _JSON_H
#define _JSON_H

#include <stdint.h>

//==============================================================================
// Type macros
//==============================================================================

#define BIT(x) (1 << (x))

#define JSON_INVALID 0
#define JSON_FALSE BIT(0)
#define JSON_TRUE BIT(1)
#define JSON_NULL BIT(2)
#define JSON_NUMBER BIT(3)
#define JSON_STRING BIT(4)
#define JSON_ARRAY BIT(5)
#define JSON_OBJECT BIT(6)
#define JSON_RAW BIT(7)

#define JSON_REFERENCE 256
#define JSON_STRINGISCONST 512

//==============================================================================
// Main JSON struct
//==============================================================================

typedef struct _json
{
    struct _json *next;
    struct _json *prev;

    struct _json *child;

    int type;

    union {
        char *value_string;
        int value_int;
        double value_double;
    };

    char *string;
} json_t;

//==============================================================================
// Allocation Hooks
//==============================================================================

typedef struct _json_hooks
{
    void *(*malloc_func)(size_t size);
    void (*free_func)(void *ptr);
} json_hooks_t;

//==============================================================================
// Provide our own boolean
//==============================================================================

typedef int json_bool_t;

//==============================================================================
// Init function (json_init.c)
//==============================================================================

void json_init_hooks(json_hooks_t *hooks);

//==============================================================================
// JSON Parse (json_parse.c)
//==============================================================================

json_t *json_parse(const char *string);
json_t *json_parse_with_opts(const char *string, const char **endptr, json_bool_t require_null_terminate);
const char *json_get_error_ptr(void);

//==============================================================================
// JSON Print (json_print.c)
//==============================================================================

char *json_print(const json_t *item);
char *json_print_unformatted(const json_t *item);
char *json_print_buffered(const json_t *item, int prebuffer, json_bool_t fmt);
json_bool_t json_print_preallocated(const json_t *item, char *buffer, const int length, const json_bool_t format);

//==============================================================================
// JSON Object (json_object.c)
//==============================================================================

json_t *json_new_item();
void json_delete(json_t *item);

json_t *json_get_object_item(const json_t *object, const char *string);
json_t *json_get_object_item_case_sensitive(const json_t *object, const char *string);
json_bool_t json_has_object_item(const json_t *object, const char *string);
json_bool_t json_is_object(const json_t *item);
json_t *json_create_object();

//==============================================================================
// JSON Array (json_array.c)
//==============================================================================

int json_get_array_size(const json_t *json);
json_t *json_get_array_item(const json_t *json);
json_bool_t *json_is_array(const json_t *item);
json_t *json_create_array();
json_t *json_create_int_array(const int *numbers, int count);
json_t *json_create_float_array(const float *numbers, int count);
json_t *json_create_double_array(const double *numbers, int count);
json_t *json_create_string_array(const char **strings, int count);
void json_add_item_to_array(json_t *array, json_t *item);
void json_add_item_reference_to_array(json_t *array, json_t *item);

//==============================================================================
// JSON Invalid (json_invalid.c)
//==============================================================================

json_bool_t
json_is_invalid(const json_t *item);

//==============================================================================
// JSON True (json_true.c)
//==============================================================================

json_bool_t json_is_true(const json_t *item);
json_t *json_create_true();

//==============================================================================
// JSON False (json_false.c)
//==============================================================================

json_bool_t json_is_false(const json_t *item);
json_t *json_create_false();

//==============================================================================
// JSON Bool (json_bool.c)
//==============================================================================

json_bool_t json_is_bool(const json_t *item);
json_t *json_create_bool(json_bool_t bool);

//==============================================================================
// JSON Null (json_null.c)
//==============================================================================

json_bool_t json_is_null(const json_t *item);
json_t *json_create_null();

//==============================================================================
// JSON Number (json_number.c)
//==============================================================================

json_bool_t json_is_number(const json_t *item);
json_t *json_create_number(double number);

//==============================================================================
// JSON String (json_string.c)
//==============================================================================

json_bool_t json_is_string(const json_t *item);
json_t *json_create_string(const char *string);
char *json_get_string_value(json_t *item);

//==============================================================================
// JSON Raw (json_raw.c)
//==============================================================================

json_bool_t json_is_raw(const json_t *item);
json_t *json_create_raw(const char *raw);

//==============================================================================
// JSON Reference (json_reference.c)
//==============================================================================

json_t *json_create_string_reference(const char *string);
json_t *json_create_object_reference(const json_t *child);
json_t *json_create_array_reference(const json_t *child);

#endif

//==============================================================================
// End of file
//==============================================================================