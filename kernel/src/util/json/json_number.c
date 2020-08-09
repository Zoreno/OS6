#include <util/json.h>

#define NAN 0.0/0.0

double json_get_number_value(const json_t *const item)
{
    if (!json_is_number(item))
    {
        return NAN;
    }

    return item->value_double;
}