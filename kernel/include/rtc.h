// Real Time Clock

#define RTC_INDEX_PORT 0x70
#define RTC_DATA_PORT  0x71

#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_C 0x0C

#define RTC_PeriodicInt 0x40
#define RTC_Clear 0x0C

#define RTC_Alarm 0x20
#define RTC_AlarmSecond 0x05
#define RTC_AlarmMinute 0x03
#define RTC_AlarmHour 0x01

#define RTC_Second 0x00
#define RTC_Minute 0x02
#define RTC_Hour 0x04
#define RTC_DayOfWeek 0x06
#define RTC_DayOfMonth 0x07
#define RTC_Month 0x08
#define RTC_Year_RH 0x09
#define RTC_Year_LH 0x32

#define RTC_Updating 0x80 
// A reg
#define RTC_BCD_Format 0x04 
// B reg


#define RTC_1024HZ 0x06
#define RTC_512HZ  0x07
#define RTC_256HZ  0x08
#define RTC_128HZ  0x09
#define RTC_64HZ   0x0A
#define RTC_32HZ   0x0B
#define RTC_16HZ   0x0C
#define RTC_8HZ    0x0D
#define RTC_4HZ    0x0E
#define RTC_2HZ    0x0F

#define RTC_IRQ 8

#include <stdint.h>


typedef struct{
    uint16_t year;
    uint8_t month;
    uint8_t day_of_month;
    uint8_t day_of_week;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} Time;

void set_rtc_handler_proc(void (*proc)());

/* IRQ8 */
void rtc_handler();

void rtc_enable_periodic_int(uint8_t rate);

void rtc_set_alarm(unsigned char hour, unsigned char minute, unsigned char second);

Time rtc_read_time();