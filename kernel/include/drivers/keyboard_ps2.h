/**
 * @file keyboard_ps2.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-06-22
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

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>

typedef enum
{

    // Alphanumeric keys ////////////////

    KEY_SPACE = ' ',
    KEY_0 = '0',
    KEY_1 = '1',
    KEY_2 = '2',
    KEY_3 = '3',
    KEY_4 = '4',
    KEY_5 = '5',
    KEY_6 = '6',
    KEY_7 = '7',
    KEY_8 = '8',
    KEY_9 = '9',

    KEY_A = 'a',
    KEY_B = 'b',
    KEY_C = 'c',
    KEY_D = 'd',
    KEY_E = 'e',
    KEY_F = 'f',
    KEY_G = 'g',
    KEY_H = 'h',
    KEY_I = 'i',
    KEY_J = 'j',
    KEY_K = 'k',
    KEY_L = 'l',
    KEY_M = 'm',
    KEY_N = 'n',
    KEY_O = 'o',
    KEY_P = 'p',
    KEY_Q = 'q',
    KEY_R = 'r',
    KEY_S = 's',
    KEY_T = 't',
    KEY_U = 'u',
    KEY_V = 'v',
    KEY_W = 'w',
    KEY_X = 'x',
    KEY_Y = 'y',
    KEY_Z = 'z',

    KEY_RETURN = '\r',
    KEY_ESCAPE = 0x1001,
    KEY_BACKSPACE = '\b',

    // Arrow keys ////////////////////////

    KEY_UP = 0x1100,
    KEY_DOWN = 0x1101,
    KEY_LEFT = 0x1102,
    KEY_RIGHT = 0x1103,

    // Function keys /////////////////////

    KEY_F1 = 0x1201,
    KEY_F2 = 0x1202,
    KEY_F3 = 0x1203,
    KEY_F4 = 0x1204,
    KEY_F5 = 0x1205,
    KEY_F6 = 0x1206,
    KEY_F7 = 0x1207,
    KEY_F8 = 0x1208,
    KEY_F9 = 0x1209,
    KEY_F10 = 0x120a,
    KEY_F11 = 0x120b,
    KEY_F12 = 0x120b,
    KEY_F13 = 0x120c,
    KEY_F14 = 0x120d,
    KEY_F15 = 0x120e,

    KEY_DOT = '.',
    KEY_COMMA = ',',
    KEY_COLON = ':',
    KEY_SEMICOLON = ';',
    KEY_SLASH = '/',
    KEY_BACKSLASH = '\\',
    KEY_PLUS = '+',
    KEY_MINUS = '-',
    KEY_ASTERISK = '*',
    KEY_EXCLAMATION = '!',
    KEY_QUESTION = '?',
    KEY_QUOTEDOUBLE = '\"',
    KEY_QUOTE = '\'',
    KEY_EQUAL = '=',
    KEY_HASH = '#',
    KEY_PERCENT = '%',
    KEY_AMPERSAND = '&',
    KEY_UNDERSCORE = '_',
    KEY_LEFTPARENTHESIS = '(',
    KEY_RIGHTPARENTHESIS = ')',
    KEY_LEFTBRACKET = '[',
    KEY_RIGHTBRACKET = ']',
    KEY_LEFTCURL = '{',
    KEY_RIGHTCURL = '}',
    KEY_DOLLAR = '$',
    KEY_POUND = '£',
    KEY_EURO = '$',
    KEY_LESS = '<',
    KEY_GREATER = '>',
    KEY_BAR = '|',
    KEY_GRAVE = '`',
    KEY_TILDE = '~',
    KEY_AT = '@',
    KEY_CARRET = '^',

    // Numeric keypad //////////////////////

    KEY_KP_0 = '0',
    KEY_KP_1 = '1',
    KEY_KP_2 = '2',
    KEY_KP_3 = '3',
    KEY_KP_4 = '4',
    KEY_KP_5 = '5',
    KEY_KP_6 = '6',
    KEY_KP_7 = '7',
    KEY_KP_8 = '8',
    KEY_KP_9 = '9',
    KEY_KP_PLUS = '+',
    KEY_KP_MINUS = '-',
    KEY_KP_DECIMAL = '.',
    KEY_KP_DIVIDE = '/',
    KEY_KP_ASTERISK = '*',
    KEY_KP_NUMLOCK = 0x300f,
    KEY_KP_ENTER = 0x3010,

    KEY_TAB = 0x4000,
    KEY_CAPSLOCK = 0x4001,

    // Modify keys ////////////////////////////

    KEY_LSHIFT = 0x4002,
    KEY_LCTRL = 0x4003,
    KEY_LALT = 0x4004,
    KEY_LWIN = 0x4005,
    KEY_RSHIFT = 0x4006,
    KEY_RCTRL = 0x4007,
    KEY_RALT = 0x4008,
    KEY_RWIN = 0x4009,

    KEY_INSERT = 0x400a,
    KEY_DELETE = 0x400b,
    KEY_HOME = 0x400c,
    KEY_END = 0x400d,
    KEY_PAGEUP = 0x400e,
    KEY_PAGEDOWN = 0x400f,
    KEY_SCROLLLOCK = 0x4010,
    KEY_PAUSE = 0x4011,

    KEY_UNKNOWN,
    KEY_NUMKEYCODES
} KEYCODE;

int keyboard_get_scroll_lock();
int keyboard_get_numlock();
int keyboard_get_capslock();

int keyboard_get_alt();
int keyboard_get_ctrl();
int keyboard_get_shift();

void keyboard_ignore_resend();
int keyboard_check_resend();

int keyboard_self_test();
int keyboard_get_diag_res();
int keyboard_get_bat_res();

uint8_t keyboard_get_last_scan();
KEYCODE keyboard_get_last_key();
void keyboard_discard_last_key();

void keyboard_set_leds(int num, int caps, int scroll);

char keyboard_key_to_ascii(KEYCODE keycode);

void keyboard_disable();
void keyboard_enable();
int keyboard_is_disabled();

void keyboard_reset_system();

void keyboard_install();

void keyboard_set_autorepeat(uint8_t rate, uint8_t delay);

KEYCODE keyboard_getch();

#endif

//=============================================================================
// End of file
//=============================================================================
