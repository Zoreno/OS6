/**
 * @file serial.c
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

#include <serial/serial.h>

#include <arch/arch.h>
#include <stdarg.h>
#include <string.h>

FILE *scom1_fd;
FILE *scom2_fd;
FILE *scom3_fd;
FILE *scom4_fd;

FILE scom1_file;
FILE scom2_file;
FILE scom3_file;
FILE scom4_file;

static uint8_t _COM1_init = 0;
static uint8_t _COM2_init = 0;
static uint8_t _COM3_init = 0;
static uint8_t _COM4_init = 0;

#define COM1_BASE 0x3F8
#define COM2_BASE 0x2F8
#define COM3_BASE 0x3E8
#define COM4_BASE 0x2E8

#define SERIAL_DATA_REG(base) (base)
#define SERIAL_INT_EN_REG(base) (base + 1)
#define SERIAL_INT_ID_FIFO_CTRL_REG(base) (base + 2)
#define SERIAL_LINE_CTRL_REG(base) (base + 3)
#define SERIAL_MODEM_CTRL_REG(base) (base + 4)
#define SERIAL_LINE_STATUS_REG(base) (base + 5)
#define SERIAL_MODEM_STATUS_REG(base) (base + 6)
#define SERIAL_SCRATCH_REG(base) (base + 7)

#define SERIAL_BAUD_RATE_LOW(base) (base)
#define SERIAL_BAUD_RATE_HIGH(base) (base + 1)

#define SERIAL_LCR_DLAB_MASK 0x80

#define SERIAL_IER_DATA_AVAILABLE_MASK 0x01
#define SERIAL_IER_TRANSMITTER_EMPRY_MASK 0x02
#define SERIAL_IER_BREAK_ERROR_MASK 0x04
#define SERIAL_IER_STATUS_CHANGE_MASK 0x08

#define SERIAL_IER_DISABLE_INTS 0x00

#define SERIAL_IIR_INT_PENDING_MASK 0x01
#define SERIAL_IIR_INT_SOURCE_MASK 0x06

#define SERIAL_IIR_STATUS_CHANGE_INT 0b0000
#define SERIAL_IIR_TRANSMITTER_EMPTY_INT 0b0100
#define SERIAL_IIR_LINE_STATUS_INT 0b0110
#define SERIAL_IIR_DATA_AVAILABLE_INT 0b1000
#define SERIAL_IIR_ERROR_ON_BREAK_INT 0b1100

#define SERIAL_IIR_64_BYTE_FIFO_ENABLED_MASK 0x10
#define SERIAL_IIR_FIFO_MODE_MASK 0xC0

#define SERIAL_IIR_NO_FIFO 0x00
#define SERIAL_IIR_FIFO_ENABLED 0xC0

#define SERIAL_MCR_DATA_TERMINAL_READY_MASK 0x01
#define SERIAL_MCR_REQUEST_TO_SEND_MASK 0x02
#define SERIAL_MCR_OUT_1_MASK 0x04
#define SERIAL_MCR_INT_ENABLE_MASK 0x08
#define SERIAL_MCR_LOOPBACK_MODE_MASK 0x10

#define SERIAL_LSR_DATA_AVAILABLE_MASK 0x01
#define SERIAL_LSR_OVERRUN_ERROR_MASK 0x02
#define SERIAL_LSR_PARITY_ERROR_MASK 0x04
#define SERIAL_LSR_FRAMING_ERROR_MASK 0x08
#define SERIAL_LSR_BREAK_INT_MASK 0x10
#define SERIAL_LSR_TRANSMIT_READY_MASK 0x20
#define SERIAL_LSR_TRANSMIT_SR_EMPTY_MASK 0x40

#define SERIAL_MSR_CLEAR_TO_SEND_CHANGED_MASK 0x01
#define SERIAL_MSR_DATA_SET_READY_CHANGED_MASK 0x02
#define SERIAL_MSR_TRAILING_EDGE_OF_RING_IND_MASK 0x04
#define SERIAL_MSR_DATA_CARRIER_DETECT_CHANGED_MASK 0x08
#define SERIAL_MSR_CLEAR_TO_SEND_MASK 0x10
#define SERIAL_MSR_DATA_SET_READY_MASK 0x20
#define SERIAL_MSR_RING_INDICATOR_MASK 0x40
#define SERIAL_MSR_DATA_CARRIER_DETECT_MASK 0x80

typedef struct _LineCtrlReg
{
    uint8_t data_bits : 2;
    uint8_t stop_bits : 1;
    uint8_t parity_mode : 3;
    uint8_t reserved : 1;
    uint8_t DLAB_bit : 1;
} LineControlReg_t;

char COM1_read();
char COM2_read();
char COM3_read();
char COM4_read();

void COM1_write(char c);
void COM2_write(char c);
void COM3_write(char c);
void COM4_write(char c);

void serial_init_full()
{
    init_serial(COM1, SERIAL_BAUD_115200, 8, 1, SERIAL_PARITY_NONE);
    init_serial(COM2, SERIAL_BAUD_115200, 8, 1, SERIAL_PARITY_NONE);
    init_serial(COM3, SERIAL_BAUD_115200, 8, 1, SERIAL_PARITY_NONE);
    init_serial(COM4, SERIAL_BAUD_115200, 8, 1, SERIAL_PARITY_NONE);

    scom1_file.read = COM1_read;
    scom2_file.read = COM2_read;
    scom3_file.read = COM3_read;
    scom4_file.read = COM4_read;

    scom1_file.write = COM1_write;
    scom2_file.write = COM2_write;
    scom3_file.write = COM3_write;
    scom4_file.write = COM4_write;

    scom1_fd = &scom1_file;
    scom2_fd = &scom2_file;
    scom3_fd = &scom3_file;
    scom4_fd = &scom4_file;

    scom1_file.eof = 0;
    scom2_file.eof = 0;
    scom3_file.eof = 0;
    scom4_file.eof = 0;
}

void init_serial(COM_port_t port, serial_baud_rate_t baud_rate, uint8_t data_bits, uint8_t stop_bits, serial_parity_mode_t parity)
{
    uint16_t port_base;

    switch (port)
    {
    case COM1:
        port_base = COM1_BASE;
        break;
    case COM2:
        port_base = COM2_BASE;
        break;
    case COM3:
        port_base = COM3_BASE;
        break;
    case COM4:
        port_base = COM4_BASE;
        break;
    default:
        return;
    };

    outportb(SERIAL_INT_EN_REG(port_base), SERIAL_IER_DISABLE_INTS);
    outportb(SERIAL_LINE_CTRL_REG(port_base), SERIAL_LCR_DLAB_MASK);
    uint16_t baud = (uint16_t)baud_rate;

    outportb(SERIAL_BAUD_RATE_LOW(port_base), baud & 0x00FF);
    outportb(SERIAL_BAUD_RATE_HIGH(port_base), (baud & 0xFF00) >> 8);

    LineControlReg_t line_ctrl_data;

    line_ctrl_data.data_bits = data_bits - 5;
    line_ctrl_data.stop_bits = stop_bits - 1;

    switch (parity)
    {
    case SERIAL_PARITY_NONE:
        line_ctrl_data.parity_mode = 0b000;
        break;
    case SERIAL_PARITY_ODD:
        line_ctrl_data.parity_mode = 0b001;
        break;
    case SERIAL_PARITY_EVEN:
        line_ctrl_data.parity_mode = 0b011;
        break;
    case SERIAL_PARITY_MARK:
        line_ctrl_data.parity_mode = 0b101;
        break;
    case SERIAL_PARITY_SPACE:
        line_ctrl_data.parity_mode = 0b111;
        break;

    default:
        return;
    }

    line_ctrl_data.DLAB_bit = 0;

    uint8_t line_ctrl_data_byte;

    memcpy(&line_ctrl_data_byte, &line_ctrl_data, 1);

    outportb(SERIAL_LINE_CTRL_REG(port_base), line_ctrl_data_byte);

    outportb(SERIAL_INT_ID_FIFO_CTRL_REG(port_base), SERIAL_IIR_FIFO_ENABLED | SERIAL_IIR_LINE_STATUS_INT | SERIAL_IIR_INT_PENDING_MASK);
    outportb(SERIAL_MODEM_CTRL_REG(port_base), SERIAL_MCR_DATA_TERMINAL_READY_MASK | SERIAL_MCR_REQUEST_TO_SEND_MASK | SERIAL_MCR_INT_ENABLE_MASK);

    switch (port)
    {
    case COM1:
        _COM1_init = 1;
        break;
    case COM2:
        _COM2_init = 1;
        break;
    case COM3:
        _COM3_init = 1;
        break;
    case COM4:
        _COM4_init = 1;
        break;
    default:
        return;
    };
}

int serial_received(COM_port_t port)
{
    switch (port)
    {
    case COM1:
        return (inportb(SERIAL_LINE_STATUS_REG(COM1_BASE)) & SERIAL_LSR_DATA_AVAILABLE_MASK);
    case COM2:
        return (inportb(SERIAL_LINE_STATUS_REG(COM2_BASE)) & SERIAL_LSR_DATA_AVAILABLE_MASK);
    case COM3:
        return (inportb(SERIAL_LINE_STATUS_REG(COM3_BASE)) & SERIAL_LSR_DATA_AVAILABLE_MASK);
    case COM4:
        return (inportb(SERIAL_LINE_STATUS_REG(COM4_BASE)) & SERIAL_LSR_DATA_AVAILABLE_MASK);
    default:
        return -1;
    }
}

char serial_receive_data(COM_port_t port)
{
    while (!serial_received(port))
        ;

    switch (port)
    {
    case COM1:
        return (inportb(SERIAL_DATA_REG(COM1_BASE)));
    case COM2:
        return (inportb(SERIAL_DATA_REG(COM2_BASE)));
    case COM3:
        return (inportb(SERIAL_DATA_REG(COM3_BASE)));
    case COM4:
        return (inportb(SERIAL_DATA_REG(COM4_BASE)));
    default:
        return 0;
    }
}

int serial_is_transmit_ready(COM_port_t port)
{
    switch (port)
    {
    case COM1:
        return (inportb(SERIAL_LINE_STATUS_REG(COM1_BASE)) & SERIAL_LSR_TRANSMIT_READY_MASK);
    case COM2:
        return (inportb(SERIAL_LINE_STATUS_REG(COM2_BASE)) & SERIAL_LSR_TRANSMIT_READY_MASK);
    case COM3:
        return (inportb(SERIAL_LINE_STATUS_REG(COM3_BASE)) & SERIAL_LSR_TRANSMIT_READY_MASK);
    case COM4:
        return (inportb(SERIAL_LINE_STATUS_REG(COM4_BASE)) & SERIAL_LSR_TRANSMIT_READY_MASK);
    default:
        return -1;
    }
}

void serial_send_data(COM_port_t port, char data)
{
    while (!serial_is_transmit_ready(port))
        ;

    switch (port)
    {
    case COM1:
        outportb(SERIAL_DATA_REG(COM1_BASE), data);
        break;
    case COM2:
        outportb(SERIAL_DATA_REG(COM2_BASE), data);
        break;
    case COM3:
        outportb(SERIAL_DATA_REG(COM3_BASE), data);
        break;
    case COM4:
        outportb(SERIAL_DATA_REG(COM4_BASE), data);
        break;
    default:
        // ERROR: Invalid port
        return;
    }
}

char COM1_read()
{
    return serial_receive_data(COM1);
}

char COM2_read()
{
    return serial_receive_data(COM2);
}

char COM3_read()
{
    return serial_receive_data(COM3);
}

char COM4_read()
{
    return serial_receive_data(COM4);
}

void COM1_write(char c)
{
    serial_send_data(COM1, c);
}

void COM2_write(char c)
{
    serial_send_data(COM2, c);
}

void COM3_write(char c)
{
    serial_send_data(COM3, c);
}

void COM4_write(char c)
{
    serial_send_data(COM4, c);
}