#ifndef MOTOR_H
#define MOTOR_H

// #include "motor.hpp"
#include <avr/io.h>
#include <avr/interrupt.h>

class Encoder {
public:
    Encoder(int motorNumber);
    int GetEncoderDist();
    void ResetEncoder();
    void incrementEncoder();

    static Encoder* encoder1;
    static Encoder* encoder2;
private:
    volatile uint16_t encCount;
    int pinA;
    int pinB;
};

#endif // MOTOR_H
