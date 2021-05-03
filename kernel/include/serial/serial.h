/**
 * @file serial.h
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

#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdint.h>
#include <stdio.h>

extern FILE *scom1_fd;
extern FILE *scom2_fd;
extern FILE *scom3_fd;
extern FILE *scom4_fd;

typedef enum
{
    COM1,
    COM2,
    COM3,
    COM4,
    NUM_COM_PORTS
} COM_port_t;

typedef enum
{
    SERIAL_BAUD_115200 = 1,
    SERIAL_BAUD_57600 = 2,
    SERIAL_BAUD_38400 = 3,
    SERIAL_BAUD_19200 = 6,
    SERIAL_BAUD_9600 = 12,
    SERIAL_BAUD_4800 = 24,
    SERIAL_BAUD_2400 = 48,
    SERIAL_BAUD_1200 = 96,
    SERIAL_BAUD_600 = 192,
    SERIAL_BAUD_300 = 384,
    SERIAL_BAUD_220 = 524,
    SERIAL_BAUD_110 = 1047,
    SERIAL_BAUD_50 = 2304,
} serial_baud_rate_t;

typedef enum
{
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN,
    SERIAL_PARITY_MARK,
    SERIAL_PARITY_SPACE
} serial_parity_mode_t;

void serial_init_full();
void init_serial(COM_port_t port,
                 serial_baud_rate_t baud_rate,
                 uint8_t data_bits,
                 uint8_t stop_bits,
                 serial_parity_mode_t parity);
int serial_received(COM_port_t port);
char serial_receive_data(COM_port_t port);
int serial_is_transmit_ready(COM_port_t port);
void serial_send_data(COM_port_t port, char data);

#endif

//=============================================================================
// End of file
//=============================================================================
