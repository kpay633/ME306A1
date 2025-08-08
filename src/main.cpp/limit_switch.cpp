#include <avr/io.h>
#include <avr/interrupt.h>
#include "limit_switch.hpp"

Limit_Switch::Limit_Switch(volatile uint8_t *ddr, volatile uint8_t *pinr, volatile uint8_t *port, uint8_t bit){
    // Set the pin as input
    this->ddr_register = ddr;
    this->pin_register = pinr;
    this->port_register = port;
    this->bit = bit;
    *(this->ddr_register) &= ~(1 << bit); // Clear the bit to set as input
    *(this->port_register) |= (1 << bit); // Enable pull-up resistor
    // Note: The port register is used to enable the pull-up resistor
    // This assumes the pin is active low, meaning it reads low when pressed
}

bool Limit_Switch::is_pressed() {
    return (*(this->pin_register) & (1 << bit)) != 0;
    // Returns true if the pin is low (pressed)
    // Returns false if the pin is high (not pressed)
}

