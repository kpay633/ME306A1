#ifndef LIMIT_SWITCH_H
#define LIMIT_SWITCH_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

class Limit_Switch {
    public:
        Limit_Switch(volatile uint8_t *ddr, volatile uint8_t *pinr, uint8_t bit){
            this->ddr_register = ddr;
            this->pin_register = pinr;
            this->bit = bit;
        }
        bool is_pressed();

    private:
        volatile uint8_t *pin_register;
        volatile uint8_t *ddr_register;
        uint8_t bit;
};

#endif // LIMIT_SWITCH_H
