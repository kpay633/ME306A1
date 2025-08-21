#include "limit_switch.hpp"

// Initialize static members

volatile bool Limit_Switch::pressed_flags[6] = {false, false, false, false, false, false};

volatile uint32_t Limit_Switch::last_interrupt_time[6] = {0, 0, 0, 0, 0, 0};

volatile uint32_t Limit_Switch::system_tick = 0;

// Debounce delay in milliseconds (adjust as needed)

const uint32_t DEBOUNCE_DELAY_MS = 50;

Limit_Switch::Limit_Switch(uint8_t int_num) {

    this->int_num = int_num;

    // Configure pin direction (input, no pull-up)

    switch (int_num) {

        case 2: DDRD &= ~(1 << PD2); PORTD &= ~(1 << PD2); break; // INT2 (Top)

        case 3: DDRD &= ~(1 << PD3); PORTD &= ~(1 << PD3); break; // INT3 (Bottom)

        case 4: DDRE &= ~(1 << PE4); PORTE &= ~(1 << PE4); break; // INT4 (Right)

        case 5: DDRE &= ~(1 << PE5); PORTE &= ~(1 << PE5); break; // INT5 (Left)

    }

    // Configure external interrupt to trigger on rising edge

    if (int_num >= 0 && int_num <= 3) {

        uint8_t shift = 2 * int_num;

        EICRA &= ~((1 << (ISC00 + shift)) | (1 << (ISC01 + shift)));

        EICRA |= (1 << (ISC00 + shift)); // logic change

    }

    // Enable the interrupt

    EIMSK |= (1 << int_num);

    

    // Initialize system timer for debouncing (Timer0, 1ms intervals)

    // This assumes you want a 1ms tick - adjust prescaler/compare value as needed

    static bool timer_initialized = false;

    if (!timer_initialized) {

        setup_system_timer();

        timer_initialized = true;

    }

}

void Limit_Switch::setup_system_timer() {

    // Setup Timer0 for 1ms interrupts (assuming 16MHz clock)

    // CTC mode, prescaler 64: (16MHz / 64) / 250 = 1000Hz (1ms)

    TCCR0A = (1 << WGM01); // CTC mode

    TCCR0B = (1 << CS01) | (1 << CS00); // Prescaler 64

    OCR0A = 249; // Compare value for 1ms at 16MHz/64

    TIMSK0 |= (1 << OCIE0A); // Enable compare interrupt

}

bool Limit_Switch::is_pressed() {

    return pressed_flags[int_num];

}

// System timer ISR for debouncing

ISR(TIMER0_COMPA_vect) {

    Limit_Switch::system_tick++;

}

// Helper function to check if enough time has passed for debouncing

bool debounce_check(uint8_t int_num) {

    uint32_t current_time = Limit_Switch::system_tick;

    if (current_time - Limit_Switch::last_interrupt_time[int_num] >= DEBOUNCE_DELAY_MS) {

        Limit_Switch::last_interrupt_time[int_num] = current_time;

        return true;

    }

    return false;

}

// ================= ISR HANDLERS =================

// Updated ISRs with debouncing

ISR(INT2_vect) { 

    if (debounce_check(2)) {

        Limit_Switch::pressed_flags[2] = (PIND & (1 << PD2)) != 0;

    }

}

ISR(INT3_vect) { 

    if (debounce_check(3)) {

        Limit_Switch::pressed_flags[3] = (PIND & (1 << PD3)) != 0;

    }

}

ISR(INT4_vect) { 

    if (debounce_check(4)) {

        Limit_Switch::pressed_flags[4] = (PINE & (1 << PE4)) != 0;

    }

}

ISR(INT5_vect) { 

    if (debounce_check(5)) {

        Limit_Switch::pressed_flags[5] = (PINE & (1 << PE5)) != 0;

    }

}