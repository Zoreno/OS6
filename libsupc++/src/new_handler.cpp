#include <new>

using std::new_handler;
namespace
{
new_handler __new_handler;
}

new_handler std::set_new_handler(new_handler handler) throw()
{
    // TODO: This should be atomic
    new_handler prev_handler = __new_handler;
    __new_handler = handler;

    return prev_handler;
}

new_handler std::get_new_handler() noexcept
{
    return __new_handler;
}