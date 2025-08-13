#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <Arduino.h>

enum class Timer {
  M1_TIMER3A,  // PE3 → OC3A → D5
  M2_TIMER4A   // PH3 → OC4A → D6
};

enum class Direction {
  CW,
  CCW
};

class Motor {
  private:
    int voltage = 0;

    Timer timer;
    int pwm_pin;
    int enc_a_pin;
    int enc_b_pin;

    int pinA;
    int pinB;

    volatile uint16_t encCount;

  public:
    Motor(int voltage, Timer timer, int pwm_pin, int enc_a_pin, int enc_b_pin);

    // int get_voltage() const;
    
    void stop_motor(Timer timer);
    void move_motor(int voltage, Direction direction, Timer timer);


    // void clockwise(int voltage, int ms);
    // void anticlockwise(int voltage, int ms);

    int GetEncoderDist();
    void ResetEncoder();
    void incrementEncoder();

    static Motor* motor1;
    static Motor* motor2;

};

#endif // MOTOR_HPP