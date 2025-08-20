#ifndef MOTOR_HPP
#define MOTOR_HPP

#include <Arduino.h>

enum class MotorID {
  M1,  // PE3 → OC3A → D5. _TIMER3A
  M2   // PH3 → OC4A → D6 _TIMER4A
};

enum class Direction {
  CW,
  CCW
};

class Motor {
  private:
    int voltage = 0;

    MotorID motorID;
    int pwm_pin;
    int enc_a_pin;
    int enc_b_pin;

    int pinA;
    int pinB;

    bool disabled;

    volatile int16_t encCount = 0;

  public:
    Motor(int voltage, MotorID motorID, int pwm_pin, int enc_a_pin, int enc_b_pin);

    // int get_voltage() const;
    
    void stop_motor(MotorID motorID);
    void move_motor(MotorID motorID, int voltage, Direction direction);
    void DisableMotor();
    void EnableMotor();

    int GetEncoderDist();
    void ResetEncoder();
    void ResetEncoder(int set_value);
    void incrementEncoder();

    static Motor* motor1;
    static Motor* motor2;

};

#endif // MOTOR_HPP