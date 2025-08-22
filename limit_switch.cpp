// LATEST WORKING VERSION

#include <avr/io.h>
#include <avr/interrupt.h>
#include "limit_switch.hpp"
#include <Arduino.h>

#define SWTOP PD2
#define SWBOTTOM PD3
#define SWRIGHT PE4
#define SWLEFT PE5



Limit_Switch::Limit_Switch(){

  DDRD &= ~((1 << PD2) | (1 << PD3)); // INT2, INT3 as input
  DDRE &= ~((1 << PE4) | (1 << PE5)); // INT4, INT5 as input

  // ---- INT2 (PD2), INT3 (PD3) are configured in EICRA ----
  EICRA |= (1 << ISC21) | (1 << ISC20); // INT2 rising edge
  EICRA |= (1 << ISC31) | (1 << ISC30); // INT3 rising edge

  // ---- INT4 (PE4), INT5 (PE5) are configured in EICRB ----
  EICRB |= (1 << ISC41) | (1 << ISC40); // INT4 rising edge
  EICRB |= (1 << ISC51) | (1 << ISC50); // INT5 rising edge

  // Enable INT2–INT5 in the mask register
  EIMSK |= (1 << INT2) | (1 << INT3) | (1 << INT4) | (1 << INT5);

}

bool Limit_Switch::is_pressed(uint8_t pin) {
    switch (pin) {
        case SWTOP:
            return (PIND & (1 << PD2)) != 0;    // INT2
        case SWBOTTOM:
            return (PIND & (1 << PD3)) != 0;    // INT3
        case SWRIGHT:
            return (PINE & (1 << PE4)) != 0;    // INT4
        case SWLEFT:
            return (PINE & (1 << PE5)) != 0;    // INT5
        default:
            return false;                        // invalid pin
    }
}