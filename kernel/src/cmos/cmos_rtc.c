/**
 * @file cmos_rtc.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2019-04-07
 * 
 * @brief Driver for the Real-time clock
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

#include <cmos/cmos_rtc.h>

#include <stdio.h>

#include <arch/arch.h>
#include <logging/logging.h>
#include <process/process.h>

//=============================================================================
// Variables
//=============================================================================

/**
 * @brief Last read seconds value
 * 
 * 
 */
static uint8_t second;

/**
 * @brief Last read minutes value
 * 
 * 
 */
static uint8_t minute;

/**
 * @brief Last read hours value
 * 
 * 
 */
static uint8_t hour;

/**
 * @brief Last read days value.
 * 
 * 
 */
static uint8_t day;

/**
 * @brief Last read months value.
 * 
 * 
 */
static uint8_t month;

/**
 * @brief Last read years value.
 * 
 * 
 */
static uint32_t year;

/**
 * @brief Current number of subticks (1/1024th of a second)
 * 
 * 
 */
static volatile uint32_t subticks;

/**
 * @brief IRQ line for the RTC
 * 
 * 
 */
#define CMOS_RTC_IRQ 8

/**
 * @brief RTC address register
 * 
 * 
 */
#define RTC_ADDR 0x70

/**
 * @brief RTC data register
 * 
 * 
 */
#define RTC_DATA 0x71

/**
 * @brief Address for the seconds register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_SECOND 0x00

/**
 * @brief Address for the alarm seconds register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_ALARM_SECOND 0x01

/**
 * @brief Address for the minute register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_MINUTE 0x02

/**
 * @brief Address for the minute alarm register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_ALARM_MINUTE 0x03

/**
 * @brief Address for the hour register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_HOUR 0x04

/**
 * @brief Address for the hour alarm register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_ALARM_HOUR 0x05

/**
 * @brief Address for the weekday register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_WEEKDAY 0x06

/**
 * @brief Address for the day register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_DAY 0x07

/**
 * @brief Address for the month register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_MONTH 0x08

/**
 * @brief Address for the year register
 * 
 * 
 */
#define RTC_REGISTER_ADDR_YEAR 0x09

/**
 * @brief Address for status register A
 * 
 * 
 */
#define RTC_REGISTER_ADDR_STATUSA 0x0A

/**
 * @brief Address for status register B
 * 
 * 
 */
#define RTC_REGISTER_ADDR_STATUSB 0x0B

/**
 * @brief Address for status register C
 * 
 * 
 */
#define RTC_REGISTER_ADDR_STATUSC 0x0C

//=============================================================================
// Types
//=============================================================================

/**
 * @brief Struct representing the contents of status register A
 * 
 * 
 */
typedef union {

    struct
    {
        /**
         * @brief Rate of the periodic interrupt
         * 
         * 
         */
        uint8_t rate_selection : 4;

        /**
         * @brief Divider for the clock
         * 
         * 
         */
        uint8_t divider : 3;

        /**
         * @brief Update in progress flag
         * 
         * 
         */
        uint8_t update_in_progress : 1;
    };

    /**
     * @brief Byte representing the status register
     * 
     * 
     */
    uint8_t byte;

} __attribute__((packed)) rtc_status_reg_a_t;

/**
 * @brief Struct representing the contents of status register B
 * 
 * 
 */
typedef union {

    struct
    {
        /**
         * @brief Daylight savings time active
         * 
         * 
         */
        uint8_t daylight_saving : 1;

        /**
         * @brief Format 24h/12h
         * 
         * 
         */
        uint8_t format_24_h : 1;

        /**
         * @brief BCD if 0, Binary if 1
         * 
         * 
         */
        uint8_t data_mode : 1;

        /**
         * @brief Square wave enabled
         * 
         * 
         */
        uint8_t square_wave : 1;

        /**
         * @brief Should fire IRQ on update?
         * 
         * 
         */
        uint8_t irq_on_update : 1;

        /**
         * @brief Should fire IRQ on alarm?
         * 
         * 
         */
        uint8_t irq_on_alarm : 1;

        /**
         * @brief Should fire periodic interrupt?
         * 
         * 
         */
        uint8_t irq_periodic : 1;

        /**
         * @brief Enable set clock mode
         * 
         * 
         */
        uint8_t set_clock : 1;
    };

    /**
     * @brief Byte representing the contents of the register.
     * 
     * 
     */
    uint8_t byte;

} __attribute__((packed)) rtc_status_reg_b_t;

/**
 * @brief Struct representing the contents of the status register C
 * 
 * 
 */
typedef union {

    struct
    {
        /**
         * @brief Reserved
         * 
         * 
         */
        uint8_t res : 4;

        /**
         * @brief Was update IRQ fired?
         * 
         * 
         */
        uint8_t update_irq : 1;

        /**
         * @brief Was alarm IRQ fired?
         * 
         * 
         */
        uint8_t alarm_irq : 1;

        /**
         * @brief Was periodic IRQ fired?
         * 
         * 
         */
        uint8_t periodic_irq : 1;

        /**
         * @brief Is an interrupt requested?
         * 
         * 
         */
        uint8_t irq_request : 1;
    };

    /**
     * @brief Byte representing the contents of the register.
     * 
     * 
     */
    uint8_t byte;

} __attribute__((packed)) rtc_status_reg_c_t;

//=============================================================================
// Forward declarations
//=============================================================================

/**
 * @brief Reads the contents of status register A
 * 
 * 
 * @return contents of status register A
 */
static rtc_status_reg_a_t read_status_a();

/**
 * @brief Reads the contents of status register B
 * 
 * 
 * @return contents of status register B
 */
static rtc_status_reg_b_t read_status_b();

/**
 * @brief Reads the contents of status register C
 * 
 * 
 * @return contents of status register C
 */
static rtc_status_reg_c_t read_status_c();

/**
 * @brief Writes @reg into status register A
 * 
 * @param reg Content to write
 * 
 */
static void write_status_a(rtc_status_reg_a_t reg);

/**
 * @brief Writes @reg into status register B
 * 
 * @param reg Content to write
 * 
 */
static void write_status_b(rtc_status_reg_b_t reg);

/**
 * @brief Reads from a CMOS register
 * 
 * @param reg Register address
 * 
 * @return Contents of the register
 */
static uint8_t read_register(uint8_t reg);

/**
 * @brief Writes to a CMOS register
 * 
 * @param reg Register address
 * @param val Value to write
 * 
 */
static void write_register(uint8_t reg, uint8_t val);

/**
 * @brief Get the update in progress flag
 * 
 * 
 * @return Non-zero if flag is set
 */
static uint8_t get_update_in_progress_flag();

/**
 * @brief Handler for the update IRQ.
 * 
 * 
 */
static void handle_update_irq();

/**
 * @brief Handler for the alarm IRQ.
 * 
 * 
 */
static void handle_alarm_irq();

/**
 * @brief Handler for the periodic IRQ.
 * 
 * 
 */
static void handle_periodic_irq();

/**
 * @brief IRQ handler for the RTC
 * 
 * @param regs System regs at the time of the interrupt.
 * 
 */
static void RTC_irq_handler(system_stack_t *regs);

/**
 * @brief Get the frequency of the periodic interrupt represented by @rate
 * 
 * @param rate Rate value from status register A
 * 
 * @return int Frequency 
 */
static int get_freq(int rate);

/**
 * @brief Calculate int value for seconds up to the given year.
 * 
 * @param years Years to account for
 * 
 * @return Seconds from 1 january 1970 to start of year.
 */
static uint32_t years_to_sec(uint32_t years);

/**
 * @brief Calculates int value for seconds up to the given month
 * 
 * @param months Months to account for
 * @param year Year
 * 
 * @return Seconds from start of year until end of given month
 */
static uint32_t months_to_sec(uint8_t months, uint8_t year);

/**
 * @brief Convert a BCD value to decimal
 * 
 * @param bcd BCD value
 * 
 * @return Decimal value
 */
static uint8_t bcd_to_dec(uint8_t bcd);

/**
 * @brief Convert a decimal value to BCD
 * 
 * @param dec Decimal value
 * 
 * @return BCD value
 */
static uint8_t dec_to_bcd(uint8_t dec);

//=============================================================================
// Private functions
//=============================================================================

static rtc_status_reg_a_t read_status_a()
{
    outportb(RTC_ADDR, RTC_REGISTER_ADDR_STATUSA);

    rtc_status_reg_a_t ret;

    ret.byte = inportb(RTC_DATA);

    return ret;
}

static rtc_status_reg_b_t read_status_b()
{
    outportb(RTC_ADDR, RTC_REGISTER_ADDR_STATUSB);

    rtc_status_reg_b_t ret;

    ret.byte = inportb(RTC_DATA);

    return ret;
}

static rtc_status_reg_c_t read_status_c()
{
    outportb(RTC_ADDR, RTC_REGISTER_ADDR_STATUSC);

    rtc_status_reg_c_t ret;

    ret.byte = inportb(RTC_DATA);

    return ret;
}

static void write_status_a(rtc_status_reg_a_t reg)
{
    // This bit is read_only
    reg.update_in_progress = 0;

    outportb(RTC_ADDR, RTC_REGISTER_ADDR_STATUSA);

    outportb(RTC_DATA, reg.byte);
}

static void write_status_b(rtc_status_reg_b_t reg)
{
    outportb(RTC_ADDR, RTC_REGISTER_ADDR_STATUSB);

    outportb(RTC_DATA, reg.byte);
}

static uint8_t read_register(uint8_t reg)
{
    outportb(RTC_ADDR, reg);

    return inportb(RTC_DATA);
}

static void write_register(uint8_t reg, uint8_t val)
{
    outportb(RTC_ADDR, reg);

    outportb(RTC_DATA, val);
}

static uint8_t get_update_in_progress_flag()
{
    rtc_status_reg_a_t status_a = read_status_a();

    return status_a.update_in_progress;
}

int timer_updated = 0;

static void handle_update_irq()
{
    static tick_count_t pit_ticks = 0;

    subticks = 0;

    ktime_t time;

    RTC_get_time(&time);

    char time_str[RTC_TIME_STR_MINLEN];

    RTC_time_to_string(time_str, &time);

    //printf("[RTC] Current time: (%s)\n", time_str);

    tick_count_t new_pit_ticks = get_tick_count();

    //printf("[RTC] Current tick rate: %d\n", new_pit_ticks - pit_ticks);

    pit_ticks = new_pit_ticks;

    timer_updated = 1;

    //printf("Timer IRQ\n");
}

static void handle_alarm_irq()
{
    printf("[RTC] Got alarm IRQ\n");
}

static void handle_periodic_irq()
{
    //printf("[RTC] Got periodic IRQ\n");
    ++subticks;
}

static void RTC_irq_handler(system_stack_t *regs)
{
    (void)regs;

    rtc_status_reg_c_t status_c;

    status_c = read_status_c();

    if (status_c.periodic_irq)
    {
        handle_periodic_irq();
    }

    else if (status_c.update_irq)
    {
        handle_update_irq();
    }

    else if (status_c.alarm_irq)
    {
        handle_alarm_irq();
    }
}

static int get_freq(int rate)
{
    return 32768 >> (rate - 1);
}

//=============================================================================
// Interface functions
//=============================================================================

void RTC_init()
{
    log_info("[RTC] Initializing...");

    rtc_status_reg_a_t status_a;

    uint8_t rate = 0b0110;

    status_a.divider = 0b010;
    status_a.rate_selection = rate;

    log_info("[RTC] Rate of periodic IRQ: %i Hz", get_freq(rate));

    write_status_a(status_a);

    rtc_status_reg_b_t status_b;

    status_b = read_status_b();

    status_b.daylight_saving = 1;
    status_b.format_24_h = 1;
    status_b.data_mode = 0; // BCD
    status_b.irq_on_update = 1;
    status_b.irq_periodic = 0;
    status_b.irq_on_alarm = 0;

    write_status_b(status_b);

    rtc_status_reg_c_t status_c;

    status_c = read_status_c();

    (void)status_c;

    set_irq_handler(CMOS_RTC_IRQ, RTC_irq_handler);

    clear_mask_interrupt(CMOS_RTC_IRQ);

    log_info("[RTC] Done!");
}

static uint8_t bcd_to_dec(uint8_t bcd)
{
    return ((bcd & 0x0F) + ((bcd & 0xF0) >> 4) * 10);
}

static uint8_t dec_to_bcd(uint8_t dec)
{
    return ((dec % 10) & 0x0F) | (((dec / 10) << 4) & 0xF0);
}

void RTC_get_time(ktime_t *time)
{
    // This is quite safe to assume
    uint8_t century = 20;

    uint8_t last_second;
    uint8_t last_minute;
    uint8_t last_hour;
    uint8_t last_day;
    uint8_t last_month;
    uint8_t last_year;

    while (get_update_in_progress_flag())
        ;

    second = read_register(RTC_REGISTER_ADDR_SECOND);
    minute = read_register(RTC_REGISTER_ADDR_MINUTE);
    hour = read_register(RTC_REGISTER_ADDR_HOUR);
    day = read_register(RTC_REGISTER_ADDR_DAY);
    month = read_register(RTC_REGISTER_ADDR_MONTH);
    year = read_register(RTC_REGISTER_ADDR_YEAR);

    // We need to repeat this process until none of the values have changed.
    // Otherwise, the data may be inconsistent
    do
    {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;

        while (get_update_in_progress_flag())
            ;

        second = read_register(RTC_REGISTER_ADDR_SECOND);
        minute = read_register(RTC_REGISTER_ADDR_MINUTE);
        hour = read_register(RTC_REGISTER_ADDR_HOUR);
        day = read_register(RTC_REGISTER_ADDR_DAY);
        month = read_register(RTC_REGISTER_ADDR_MONTH);
        year = read_register(RTC_REGISTER_ADDR_YEAR);

    } while ((last_second != second) ||
             (last_minute != minute) ||
             (last_hour != hour) ||
             (last_day != day) ||
             (last_month != month) ||
             (last_year != year));

    rtc_status_reg_b_t status_b;

    status_b = read_status_b();

    if (status_b.data_mode == 0)
    {
        second = bcd_to_dec(second);
        minute = bcd_to_dec(minute);
        hour = bcd_to_dec(hour & 0x7F) | (hour & 0x80);
        day = bcd_to_dec(day);
        month = bcd_to_dec(month);
        year = bcd_to_dec(year);
    }

    if (status_b.format_24_h == 0 && (hour & 0x80))
    {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    year += 100 * century;

    time->second = second;
    time->minute = minute;
    time->hour = hour & 0x7F;
    time->day = day;
    time->month = month;
    time->year = year;
}

void RTC_set_time(ktime_t *time)
{
    rtc_status_reg_b_t status_b;

    // Set the SET flag
    status_b = read_status_b();

    status_b.set_clock = 1;

    write_status_b(status_b);

    if (status_b.data_mode == 0)
    {

        write_register(RTC_REGISTER_ADDR_SECOND, dec_to_bcd(time->second));
        write_register(RTC_REGISTER_ADDR_MINUTE, dec_to_bcd(time->minute));

        if (status_b.format_24_h == 1)
        {
            write_register(RTC_REGISTER_ADDR_HOUR, dec_to_bcd(time->hour));
        }
        else
        {
            uint8_t hour_val = time->hour;
            uint8_t pm = 0;

            if (hour_val > 12)
            {
                hour_val -= 12;
                pm = 0x80;
            }

            if (hour_val == 0)
            {
                hour_val = 12;
            }

            write_register(RTC_REGISTER_ADDR_HOUR, dec_to_bcd((hour_val & 0x7F) | (pm)));
        }

        write_register(RTC_REGISTER_ADDR_DAY, dec_to_bcd(time->day));
        write_register(RTC_REGISTER_ADDR_MONTH, dec_to_bcd(time->month));
        write_register(RTC_REGISTER_ADDR_YEAR, dec_to_bcd(time->year - 2000));
    }
    else
    {
        write_register(RTC_REGISTER_ADDR_SECOND, time->second);
        write_register(RTC_REGISTER_ADDR_MINUTE, time->minute);

        if (status_b.format_24_h == 1)
        {
            write_register(RTC_REGISTER_ADDR_HOUR, time->hour);
        }
        else
        {
            uint8_t hour_val = time->hour;
            uint8_t pm = 0;

            if (hour_val > 12)
            {
                hour_val -= 12;
                pm = 0x80;
            }

            if (hour_val == 0)
            {
                hour_val = 12;
            }

            write_register(RTC_REGISTER_ADDR_HOUR, (hour_val & 0x7F) | (pm));
        }

        write_register(RTC_REGISTER_ADDR_DAY, time->day);
        write_register(RTC_REGISTER_ADDR_MONTH, time->month);
        write_register(RTC_REGISTER_ADDR_YEAR, time->year - 2000);
    }

    // Clear the SET flag
    status_b = read_status_b();

    status_b.set_clock = 0;

    write_status_b(status_b);
}

// TODO: Implement
void RTC_set_alarm(ktime_t *time);
int RTC_get_alarm(ktime_t *time);
void RTC_abort_alarm();

void RTC_time_to_string(char *str, ktime_t *time)
{
    ktime_t *time_ptr;
    ktime_t time_struct;

    if (time == NULL)
    {
        RTC_get_time(&time_struct);
        time_ptr = &time_struct;
    }
    else
    {
        time_ptr = time;
    }

    // TODO: Implement more formats for the time string
    sprintf(str, "%02i:%02i:%02i %04i-%02i-%02i",
            time_ptr->hour, time_ptr->minute, time_ptr->second,
            time_ptr->year, time_ptr->month, time_ptr->day);
}

static const uint32_t secs_per_day = 24 * 60 * 60;
static const uint32_t secs_per_hour = 60 * 60;
static const uint32_t secs_per_minute = 60;

static uint32_t years_to_sec(uint32_t years)
{
    uint32_t days = 0;

    while (years > 1969)
    {
        days += 365;

        if (years % 4 == 0)
        {
            if (years % 100 == 0)
            {
                if (years % 400 == 0)
                {
                    ++days;
                }
            }
            else
            {
                ++days;
            }
        }

        --years;
    }

    return days * secs_per_day;
}

static uint32_t months_to_sec(uint8_t months, uint8_t year)
{
    uint32_t days = 0;

    switch (months)
    {
    case 11:
        days += 30;
        __attribute__((fallthrough));
    case 10:
        days += 31;
        __attribute__((fallthrough));
    case 9:
        days += 30;
        __attribute__((fallthrough));
    case 8:
        days += 31;
        __attribute__((fallthrough));
    case 7:
        days += 31;
        __attribute__((fallthrough));
    case 6:
        days += 30;
        __attribute__((fallthrough));
    case 5:
        days += 31;
        __attribute__((fallthrough));
    case 4:
        days += 30;
        __attribute__((fallthrough));
    case 3:
        days += 31;
        __attribute__((fallthrough));
    case 2:
        days += 28;
        if ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)))
        {
            days++;
        }
        __attribute__((fallthrough));
    case 1:
        days += 31;
        __attribute__((fallthrough));
    default:
        break;
    }

    return days * secs_per_day;
}

uint32_t RTC_time_to_int(ktime_t *time)
{
    uint32_t time_int = 0;

    // Years
    time_int += years_to_sec(time->year - 1);

    // Months
    time_int += months_to_sec(time->month - 1, time->year);

    // Days
    time_int += (time->day - 1) * secs_per_day;

    // Hours
    time_int += time->hour * secs_per_hour;

    // Minutes
    time_int += time->minute * secs_per_minute;

    // Seconds
    time_int += time->second;

    return time_int;
}

void RTC_int_to_time(uint32_t time_int, ktime_t *time)
{
    uint32_t year = 1970;
    uint8_t month = 1;
    uint8_t day = 1;
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t second = 0;

    while (time_int > years_to_sec(year))
    {
        ++year;
    }

    time_int -= years_to_sec(year - 1);

    while (time_int > months_to_sec(month, year))
    {
        ++month;
    }

    time_int -= months_to_sec(month - 1, year);

    day = time_int / secs_per_day + 1;

    time_int %= secs_per_day;

    hour = time_int / secs_per_hour;

    time_int %= secs_per_hour;

    minute = time_int / secs_per_minute;
    second = time_int % secs_per_minute;

    time->year = year;
    time->month = month;
    time->day = day;
    time->hour = hour;
    time->minute = minute;
    time->second = second;
}

//=============================================================================
// End of file
//=============================================================================