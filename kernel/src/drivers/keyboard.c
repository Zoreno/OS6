#include <drivers/keyboard.h>

#include <arch/arch.h>

#include <string.h>
#include <stdio.h>
#include <ctype.h>

enum KEYBOARD_ENCODER_IO
{

    KEYBOARD_ENC_INPUT_BUF = 0x60,
    KEYBOARD_ENC_CMD_REG = 0x60
};

enum KEYBOARD_ENC_CMDS
{

    KEYBOARD_ENC_CMD_SET_LED = 0xED,
    KEYBOARD_ENC_CMD_ECHO = 0xEE,
    KEYBOARD_ENC_CMD_SCAN_CODE_SET = 0xF0,
    KEYBOARD_ENC_CMD_ID = 0xF2,
    KEYBOARD_ENC_CMD_AUTODELAY = 0xF3,
    KEYBOARD_ENC_CMD_ENABLE = 0xF4,
    KEYBOARD_ENC_CMD_RESETWAIT = 0xF5,
    KEYBOARD_ENC_CMD_RESETSCAN = 0xF6,
    KEYBOARD_ENC_CMD_ALL_AUTO = 0xF7,
    KEYBOARD_ENC_CMD_ALL_MAKEBREAK = 0xF8,
    KEYBOARD_ENC_CMD_ALL_MAKEONLY = 0xF9,
    KEYBOARD_ENC_CMD_ALL_MAKEBREAK_AUTO = 0xFA,
    KEYBOARD_ENC_CMD_SINGLE_AUTOREPEAT = 0xFB,
    KEYBOARD_ENC_CMD_SINGLE_MAKEBREAK = 0xFC,
    KEYBOARD_ENC_CMD_SINGLE_BREAKONLY = 0xFD,
    KEYBOARD_ENC_CMD_RESEND = 0xFE,
    KEYBOARD_ENC_CMD_RESET = 0xFF
};

enum KEYBOARD_CTRL_IO
{

    KEYBOARD_CTRL_STATS_REG = 0x64,
    KEYBOARD_CTRL_CMD_REG = 0x64
};

enum KEYBOARD_CTRL_STATS_MASK
{

    KEYBOARD_CTRL_STATS_MASK_OUT_BUF = 1,    //00000001
    KEYBOARD_CTRL_STATS_MASK_IN_BUF = 2,     //00000010
    KEYBOARD_CTRL_STATS_MASK_SYSTEM = 4,     //00000100
    KEYBOARD_CTRL_STATS_MASK_CMD_DATA = 8,   //00001000
    KEYBOARD_CTRL_STATS_MASK_LOCKED = 0x10,  //00010000
    KEYBOARD_CTRL_STATS_MASK_AUX_BUF = 0x20, //00100000
    KEYBOARD_CTRL_STATS_MASK_TIMEOUT = 0x40, //01000000
    KEYBOARD_CTRL_STATS_MASK_PARITY = 0x80   //10000000
};

enum KEYBOARD_CTRL_CMDS
{

    KEYBOARD_CTRL_CMD_READ = 0x20,
    KEYBOARD_CTRL_CMD_WRITE = 0x60,
    KEYBOARD_CTRL_CMD_SELF_TEST = 0xAA,
    KEYBOARD_CTRL_CMD_INTERFACE_TEST = 0xAB,
    KEYBOARD_CTRL_CMD_DISABLE = 0xAD,
    KEYBOARD_CTRL_CMD_ENABLE = 0xAE,
    KEYBOARD_CTRL_CMD_READ_IN_PORT = 0xC0,
    KEYBOARD_CTRL_CMD_READ_OUT_PORT = 0xD0,
    KEYBOARD_CTRL_CMD_WRITE_OUT_PORT = 0xD1,
    KEYBOARD_CTRL_CMD_READ_TEST_INPUTS = 0xE0,
    KEYBOARD_CTRL_CMD_SYSTEM_RESET = 0xFE,
    KEYBOARD_CTRL_CMD_MOUSE_DISABLE = 0xA7,
    KEYBOARD_CTRL_CMD_MOUSE_ENABLE = 0xA8,
    KEYBOARD_CTRL_CMD_MOUSE_PORT_TEST = 0xA9,
    KEYBOARD_CTRL_CMD_MOUSE_WRITE = 0xD4
};

enum KEYBOARD_ERROR
{

    KEYBOARD_ERR_BUF_OVERRUN = 0,
    KEYBOARD_ERR_ID_RET = 0x83AB,
    KEYBOARD_ERR_BAT = 0xAA, //note: can also be L. shift key make code
    KEYBOARD_ERR_ECHO_RET = 0xEE,
    KEYBOARD_ERR_ACK = 0xFA,
    KEYBOARD_ERR_BAT_FAILED = 0xFC,
    KEYBOARD_ERR_DIAG_FAILED = 0xFD,
    KEYBOARD_ERR_RESEND_CMD = 0xFE,
    KEYBOARD_ERR_KEY = 0xFF
};

typedef struct
{

    uint16_t num_lock : 1;
    uint16_t scroll_lock : 1;
    uint16_t caps_lock : 1;

    uint16_t shift : 1;
    uint16_t alt : 1;
    uint16_t ctrl : 1;

    uint16_t bat_res : 1;
    uint16_t diag_res : 1;
    uint16_t resend_res : 1;

    uint16_t enabled : 1;

    uint16_t reserved : 6;

} keyboard_status_t;

static volatile char _scancode;

static keyboard_status_t _status;

static int _keyboard_error = 0;

static int _keyboard_scancode_std[] = {

    //! key			scancode
    KEY_UNKNOWN,      //0
    KEY_ESCAPE,       //1
    KEY_1,            //2
    KEY_2,            //3
    KEY_3,            //4
    KEY_4,            //5
    KEY_5,            //6
    KEY_6,            //7
    KEY_7,            //8
    KEY_8,            //9
    KEY_9,            //0xa
    KEY_0,            //0xb
    KEY_MINUS,        //0xc
    KEY_EQUAL,        //0xd
    KEY_BACKSPACE,    //0xe
    KEY_TAB,          //0xf
    KEY_Q,            //0x10
    KEY_W,            //0x11
    KEY_E,            //0x12
    KEY_R,            //0x13
    KEY_T,            //0x14
    KEY_Y,            //0x15
    KEY_U,            //0x16
    KEY_I,            //0x17
    KEY_O,            //0x18
    KEY_P,            //0x19
    KEY_LEFTBRACKET,  //0x1a
    KEY_RIGHTBRACKET, //0x1b
    KEY_RETURN,       //0x1c
    KEY_LCTRL,        //0x1d
    KEY_A,            //0x1e
    KEY_S,            //0x1f
    KEY_D,            //0x20
    KEY_F,            //0x21
    KEY_G,            //0x22
    KEY_H,            //0x23
    KEY_J,            //0x24
    KEY_K,            //0x25
    KEY_L,            //0x26
    KEY_SEMICOLON,    //0x27
    KEY_QUOTE,        //0x28
    KEY_GRAVE,        //0x29
    KEY_LSHIFT,       //0x2a
    KEY_BACKSLASH,    //0x2b
    KEY_Z,            //0x2c
    KEY_X,            //0x2d
    KEY_C,            //0x2e
    KEY_V,            //0x2f
    KEY_B,            //0x30
    KEY_N,            //0x31
    KEY_M,            //0x32
    KEY_COMMA,        //0x33
    KEY_DOT,          //0x34
    KEY_SLASH,        //0x35
    KEY_RSHIFT,       //0x36
    KEY_KP_ASTERISK,  //0x37
    KEY_RALT,         //0x38
    KEY_SPACE,        //0x39
    KEY_CAPSLOCK,     //0x3a
    KEY_F1,           //0x3b
    KEY_F2,           //0x3c
    KEY_F3,           //0x3d
    KEY_F4,           //0x3e
    KEY_F5,           //0x3f
    KEY_F6,           //0x40
    KEY_F7,           //0x41
    KEY_F8,           //0x42
    KEY_F9,           //0x43
    KEY_F10,          //0x44
    KEY_KP_NUMLOCK,   //0x45
    KEY_SCROLLLOCK,   //0x46
    KEY_HOME,         //0x47
    KEY_KP_8,         //0x48	//keypad up arrow
    KEY_PAGEUP,       //0x49
    KEY_KP_2,         //0x50	//keypad down arrow
    KEY_KP_3,         //0x51	//keypad page down
    KEY_KP_0,         //0x52	//keypad insert key
    KEY_KP_DECIMAL,   //0x53	//keypad delete key
    KEY_UNKNOWN,      //0x54
    KEY_UNKNOWN,      //0x55
    KEY_UNKNOWN,      //0x56
    KEY_F11,          //0x57
    KEY_F12           //0x58
};

const int INVALID_SCANCODE = 0;

uint8_t keyboard_ctrl_read_status();
void keyboard_ctrl_send_command(uint8_t cmd);

uint8_t keyboard_enc_read_buffer();
void keyboard_enc_send_command(uint8_t cmd);

void i86_keyboard_irq();

#define TOGGLE(x) (x = (x) ? 0 : 1)

uint8_t keyboard_ctrl_read_status()
{

    // Simply read status register
    return inportb(KEYBOARD_CTRL_STATS_REG);
}

void keyboard_ctrl_send_command(uint8_t cmd)
{

    // Wait until controller is ready to accept command.
    while (1)
    {
        if ((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_IN_BUF) == 0)
            break;
    }

    // Send command to controller command register
    outportb(KEYBOARD_CTRL_CMD_REG, cmd);
}

uint8_t keyboard_enc_read_buffer()
{

    // Simply read encoder buffer
    return inportb(KEYBOARD_ENC_INPUT_BUF);
}

void keyboard_enc_send_command(uint8_t cmd)
{

    // Wait until controller is ready to accept command.
    while (1)
    {
        if ((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_IN_BUF) == 0)
            break;
    }

    // Send command to encoder command register
    outportb(KEYBOARD_ENC_CMD_REG, cmd);
}

void keyboard_irq_handler(system_stack_t *regs)
{
    (void)regs;

    static int _extended = 0;

    int code = 0;

    // Check if there is anything in output buffer
    if (keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_OUT_BUF)
    {

        // Read scan code
        code = keyboard_enc_read_buffer();

        if (code == 0xE0 || code == 0xE1)
        {

            _extended = 1;
        }
        else
        {

            _extended = 0;

            // Check if key is a break code(key up code)
            if (code & 0x80)
            {
                code -= 0x80;

                int key = _keyboard_scancode_std[code];

                // Check if key was a special key
                switch (key)
                {

                case KEY_LCTRL:
                case KEY_RCTRL:
                    _status.ctrl = 0;
                    break;

                case KEY_LSHIFT:
                case KEY_RSHIFT:
                    _status.shift = 0;
                    break;

                case KEY_LALT:
                case KEY_RALT:
                    _status.alt = 0;
                    break;
                }
            }
            else
            { // Make code (Key down code)

                _scancode = code;

                int key = _keyboard_scancode_std[code];

                switch (key)
                {

                case KEY_LCTRL:
                case KEY_RCTRL:
                    _status.ctrl = 1;
                    break;

                case KEY_LSHIFT:
                case KEY_RSHIFT:
                    _status.shift = 1;
                    break;

                case KEY_LALT:
                case KEY_RALT:
                    _status.alt = 1;
                    break;

                case KEY_CAPSLOCK:
                    TOGGLE(_status.caps_lock);
                    keyboard_set_leds(
                        _status.num_lock,
                        _status.caps_lock,
                        _status.scroll_lock);
                    break;

                case KEY_KP_NUMLOCK:
                    TOGGLE(_status.num_lock);
                    keyboard_set_leds(
                        _status.num_lock,
                        _status.caps_lock,
                        _status.scroll_lock);
                    break;

                case KEY_SCROLLLOCK:
                    TOGGLE(_status.scroll_lock);
                    keyboard_set_leds(
                        _status.num_lock,
                        _status.caps_lock,
                        _status.scroll_lock);
                    break;
                }
            }
        }

        switch (code)
        {

        case KEYBOARD_ERR_BAT_FAILED:
            _status.bat_res = 0;
            break;

        case KEYBOARD_ERR_DIAG_FAILED:
            _status.diag_res = 0;
            break;

        case KEYBOARD_ERR_RESEND_CMD:
            _status.resend_res = 1;
            break;
        }
    }
}

int keyboard_get_scroll_lock()
{
    return _status.scroll_lock;
}

int keyboard_get_numlock()
{
    return _status.num_lock;
}

int keyboard_get_capslock()
{
    return _status.caps_lock;
}

int keyboard_get_alt()
{
    return _status.alt;
}

int keyboard_get_ctrl()
{
    return _status.ctrl;
}

int keyboard_get_shift()
{
    return _status.shift;
}

void keyboard_ignore_resend()
{
    _status.resend_res = 0;
}

int keyboard_check_resend()
{
    return _status.resend_res;
}

int keyboard_self_test()
{

    // Send self test command.
    keyboard_ctrl_send_command(KEYBOARD_CTRL_CMD_SELF_TEST);

    // Wait until data is ready
    while (1)
    {
        if (keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_OUT_BUF)
        {
            break;
        }
    }

    // If data is 0x55, self test was passed.
    return (keyboard_enc_read_buffer() == 0x55) ? 1 : 0;
}

int keyboard_get_diag_res()
{
    return _status.diag_res;
}

int keyboard_get_bat_res()
{
    return _status.bat_res;
}

uint8_t keyboard_get_last_scan()
{
    return _scancode;
}

KEYCODE keyboard_get_last_key()
{
    // Check if scan code is valid
    return (_scancode != INVALID_SCANCODE) ? ((KEYCODE)_keyboard_scancode_std[_scancode]) : (KEY_UNKNOWN);
}

void keyboard_discard_last_key()
{
    _scancode = INVALID_SCANCODE;
}

void keyboard_set_leds(int num, int caps, int scroll)
{
    uint8_t data = 0;

    if (num)
    {
        data |= (1 << 1);
    }
    if (caps)
    {
        data |= (1 << 2);
    }
    if (scroll)
    {
        data |= (1 << 0);
    }

    keyboard_enc_send_command(KEYBOARD_ENC_CMD_SET_LED);
    keyboard_enc_send_command(data);
}

char keyboard_key_to_ascii(KEYCODE keycode)
{

    uint8_t key = keycode;

    if (isascii(key))
    {

        // TODO: change to XOR
        if (_status.shift || _status.caps_lock)
        {
            // Check if key is in a-z range
            if (key >= 'a' && key <= 'z')
            {
                key -= 32;
            }
        }

        if (_status.shift && !_status.caps_lock)
        {
            if (key >= '0' && key <= '9')
            {
                switch (key)
                {

                case '0':
                    key = KEY_EQUAL;
                    break;
                case '1':
                    key = KEY_EXCLAMATION;
                    break;
                case '2':
                    key = KEY_QUOTEDOUBLE;
                    break;
                case '3':
                    key = KEY_HASH;
                    break;
                case '4':
                    key = KEY_GRAVE;
                    break;
                case '5':
                    key = KEY_PERCENT;
                    break;
                case '6':
                    key = KEY_AMPERSAND;
                    break;
                case '7':
                    key = KEY_SLASH;
                    break;
                case '8':
                    key = KEY_LEFTPARENTHESIS;
                    break;
                case '9':
                    key = KEY_RIGHTPARENTHESIS;
                    break;
                }
            }
            else
            {
                switch (key)
                {
                case KEY_COMMA:
                    key = KEY_SEMICOLON;
                    break;

                case KEY_DOT:
                    key = KEY_COLON;
                    break;

                case KEY_QUOTE:
                    key = KEY_KP_ASTERISK;
                    break;

                case KEY_TILDE:
                    key = KEY_CARRET;
                    break;

                case KEY_RIGHTBRACKET:
                    key = KEY_RIGHTCURL;
                    break;

                case KEY_GRAVE:
                    key = KEY_TILDE;
                    break;

                case KEY_MINUS:
                    key = KEY_UNDERSCORE;
                    break;

                case KEY_PLUS:
                    key = KEY_EQUAL;
                    break;

                case KEY_BACKSLASH:
                    key = KEY_BAR;
                    break;
                }
            }
        }

        return key;
    }
    // Scan code is not an ascii character
    return 0;
}

void keyboard_disable()
{
    keyboard_ctrl_send_command(KEYBOARD_CTRL_CMD_DISABLE);
    _status.enabled = 0;
}

void keyboard_enable()
{
    keyboard_ctrl_send_command(KEYBOARD_CTRL_CMD_ENABLE);
    _status.enabled = 1;
}

int keyboard_is_disabled()
{
    return (_status.enabled) ? 0 : 1;
}

void keyboard_reset_system()
{
    keyboard_ctrl_send_command(KEYBOARD_CTRL_CMD_WRITE_OUT_PORT);
    keyboard_enc_send_command(0xFE);
}

void keyboard_install()
{
    printf("[KEYBOARD] Installing\n");

    _status.bat_res = 1;
    _scancode = 0;

    _status.num_lock = 0;
    _status.caps_lock = 0;
    _status.scroll_lock = 0;

    keyboard_set_leds(0, 0, 0);

    _status.shift = 0;
    _status.alt = 0;
    _status.ctrl = 0;

    set_irq_handler(1, keyboard_irq_handler);
    clear_mask_interrupt(1);

    printf("[KEYBOARD] Done\n");
}

void keyboard_set_autorepeat(uint8_t rate, uint8_t delay)
{

    // D - Delay
    // R - Rate
    // 0DDRRRRR

    uint8_t data = rate & 0x1F;
    data |= (delay & 0x03) << 5;

    keyboard_enc_send_command(KEYBOARD_ENC_CMD_AUTODELAY);
    keyboard_enc_send_command(data);
}

KEYCODE keyboard_getch()
{
    KEYCODE c;

    while ((c = keyboard_get_last_key()) == KEY_UNKNOWN)
        ;

    keyboard_discard_last_key();

    return c;
}