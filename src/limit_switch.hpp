#ifndef LIMIT_SWITCH_H
#define LIMIT_SWITCH_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

enum class switchPos {
    Left, Right, Top, Bottom
};

class Limit_Switch {
    public:
        Limit_Switch();
        bool is_pressed(switchPos switch_pos);
        
        static volatile uint8_t switch_state;
    private:
};

#endif // LIMIT_SWITCH_H