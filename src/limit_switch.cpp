#include <avr/io.h>
#include <avr/interrupt.h>
#include "limit_switch.hpp"
#include <Arduino.h>
#include "motor.hpp"

volatile uint8_t Limit_Switch::switch_state = 0b00;

Limit_Switch::Limit_Switch(){
    // // Set the pin as input

    // *(this->port_register) |= (1 << bit); // Enable pull-up resistor
    // // Note: The port register is used to enable the pull-up resistor
    // // This assumes the pin is active low, meaning it reads low when pressed

    //Disable global interupts during setup
    cli();

    //Set digital pins 18-21 to input
    DDRD &= ~((1 << PD2) | (1 << PD3)); //(1 << PD0) | (1 << PD1) | 

    //Enable internal pull-ups on pins 18-21
    PORTD |= (1 << PD2) | (1 << PD3); //(1 << PD0) | (1 << PD1) |

    //Configure INT0-INT3 to trigger on rising edge
    EICRA |= (1 << ISC20) | (1 << ISC21) | (1 << ISC30) | (1 << ISC31); //(1 << ISC00) | (1 << ISC01) | (1 << ISC10) | (1 << ISC11) | 

    //Enable interupts 0-3
    EIMSK |= (1 << INT2) | (1 << INT3); //(1 << INT0) | (1 << INT1) |

    //Re-enable global interupts
    sei();
    
}

bool Limit_Switch::is_pressed(switchPos switch_pos) {
    // Returns true if the pin is low (pressed)
    // Returns false if the pin is high (not pressed)

    switch (switch_pos) {
        case switchPos::Left:   return !(PIND & (1 << PD2)); // pin 19
        case switchPos::Right:  return !(PIND & (1 << PD2)); // pin 19
        case switchPos::Top:    return !(PIND & (1 << PD3)); // pin 18
        case switchPos::Bottom: return !(PIND & (1 << PD3)); // pin 18
        default: return false;
    }
}

