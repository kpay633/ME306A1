#include <avr/io.h>
#include <avr/interrupt.h>
#include "limit_switch.hpp"

Limit_Switch::Limit_Switch(volatile uint8_t *ddr, volatile uint8_t *pinr, uint8_t bit){
    // Set the pin as input
    this->ddr_register = ddr;
    this->pin_register = pinr;
    this->bit = bit;
}

bool Limit_Switch::is_pressed() {
    return this->pin_register & (1 << bit);
}

