#include "rtc.h"
#include "pic.h"
#include "low_level.h"
#include "apic.h"


static uint8_t rtc_read(int8_t reg) {
    outb(RTC_INDEX_PORT, reg);      // Select the register
    return inb(RTC_DATA_PORT);     // Read the value from the register
}

static void rtc_write(int8_t reg, int8_t value) {
    outb(RTC_INDEX_PORT, reg);      // Select the register
    outb(RTC_DATA_PORT, value);    // Write the value to the register
}

void (*rtc_handler_proc)();

void set_rtc_handler_proc(void (*proc)()){
    rtc_handler_proc = proc;
}

/* IRQ8 */
void rtc_handler() {
    // Read from the RTC to clear the interrupt
    rtc_read(RTC_Clear);
    
    // Perform necessary timekeeping or periodic tasks
    if(rtc_handler_proc)
        rtc_handler_proc();

    // Send End of Interrupt (EOI) signal to the PIC
    pic_sendEOI(RTC_IRQ);
}

void rtc_enable_periodic_int(uint8_t rate) {
    // Disable interrupts
    asm volatile("cli");
    
    // Select the B register and disable the NMI
    int8_t prev = rtc_read(RTC_REG_B); // Read the current value of B register
    rtc_write(RTC_REG_B, prev | RTC_PeriodicInt);  // Set the periodic interrupt enable bit

    // Set Period
    prev = rtc_read(RTC_REG_A);
    rtc_write(RTC_REG_A, (prev & 0xF0) | (rate & 0x0F));

    // Renable NMI
    rtc_read(RTC_Clear);

    // Re-enable interrupts
    asm volatile("sti");
}

void rtc_set_alarm(uint8_t hour, uint8_t minute, uint8_t second) {
    rtc_write(RTC_AlarmSecond, second % 60);  // Set seconds
    rtc_write(RTC_AlarmMinute, minute % 60);  // Set minutes
    rtc_write(RTC_AlarmHour, hour % 24);    // Set hours
    uint8_t reg_b = rtc_read(RTC_REG_B);
    rtc_write(RTC_REG_B, reg_b | RTC_Alarm);  // Enable alarm interrupts
}

// Convert BCD to binary
static uint8_t bcd_to_bin(uint8_t bcd) {
    return ((bcd / 16) * 10) + (bcd % 16);
}

time_t rtc_read_time(){
    time_t time;
    uint8_t reg_a;
    uint8_t year_rh, year_lh;

    // Wait until RTC is not updating
    do {
        reg_a = rtc_read(RTC_REG_A);
    } while (reg_a & RTC_Updating);

    time.second = rtc_read(RTC_Second);
    time.minute = rtc_read(RTC_Minute);
    time.hour = rtc_read(RTC_Hour);
    time.day_of_week = rtc_read(RTC_DayOfWeek);
    time.day_of_month = rtc_read(RTC_DayOfMonth);
    time.month = rtc_read(RTC_Month);
    year_rh = rtc_read(RTC_Year_RH);
    year_lh = rtc_read(RTC_Year_LH);
    time.year = year_lh * 100 + year_rh;

    
    uint8_t reg_b = rtc_read(RTC_REG_B);
    if (!(reg_b & 0x04)) {
        // Convert BCD to binary
        time.second = bcd_to_bin(time.second);
        time.minute = bcd_to_bin(time.minute);
        time.hour = bcd_to_bin(time.hour);
        time.day_of_week = bcd_to_bin(time.day_of_week);
        time.day_of_month = bcd_to_bin(time.day_of_month);
        time.month = bcd_to_bin(time.month);
        year_rh = bcd_to_bin(year_rh);
        year_lh = bcd_to_bin(year_lh);
        time.year = year_lh * 100 + year_rh;
    }

    return time;
}

void apic_rtc_handler() {
    // Read from the RTC to clear the interrupt
    rtc_read(RTC_Clear);
    
    // Perform necessary timekeeping or periodic tasks
    if(rtc_handler_proc)
        rtc_handler_proc();

    // Send End of Interrupt (EOI) signal to the PIC
    apic_sendEOI();
}