#ifndef MOTOR_HPP
#define MOTOR_HPP
#include "encoder.hpp"

class Encoder;

enum class Timer {
  TIMER3A,  // PE3 → OC3A → D5
  TIMER4A   // PH3 → OC4A → D6
};

class Motor {
  private:
    int voltage = 0;

    Timer timer;
    int pwm_pin;
    int enc_a_pin;
    int enc_b_pin;
    Encoder* myEncoder;
    int motorNumber;

  public:
    Motor(int voltage, Timer timer, int pwm_pin, int enc_a_pin, int enc_b_pin);

    int get_voltage() const;
    void move_motor(int voltage);
    int GetEncoderDist();
};

#endif // MOTOR_HPP