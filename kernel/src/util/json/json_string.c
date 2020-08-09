#include <util/json.h>

#include <stdint.h>
#include <stddef.h>

char *json_get_string_value(const json_t *const item)
{
    if (!json_is_string(item))
    {
        return NULL;
    }

    return item->value_string;
}