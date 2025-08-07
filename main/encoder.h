#ifndef MOTOR_H
#define MOTOR_H

// #include "motor.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>

class Encoder {
public:
    Encoder(int PinA, int PinB);
    int GetEncoderDist();
    void incrementEncoder();
private:
    volatile uint16_t encCount;
    int pinA;
    int pinB;
};

#endif // MOTOR_H
