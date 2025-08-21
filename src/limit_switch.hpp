#ifndef LIMIT_SWITCH_HPP
#define LIMIT_SWITCH_HPP

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

class Limit_Switch {
public:
    Limit_Switch(uint8_t int_num);
    bool is_pressed();
    
    // Public static members for ISR access
    static volatile bool pressed_flags[6];
    static volatile uint32_t last_interrupt_time[6];
    static volatile uint32_t system_tick;

private:
    uint8_t int_num;
    static void setup_system_timer();
};

#endif // LIMIT_SWITCH_HPP
