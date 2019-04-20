#include <usb/usb_controller.h>

usb_controller_t *_usb_controller_list;

usb_controller_t *usb_get_controller_list()
{
    return _usb_controller_list;
}

void usb_set_controller_list(usb_controller_t *new_controller_list)
{
    _usb_controller_list = new_controller_list;
}