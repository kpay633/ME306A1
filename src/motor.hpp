#ifndef MOTOR_HPP
#define MOTOR_HPP

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

<<<<<<< HEAD
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
=======
  public:
    Motor(int voltage, Timer timer, int pwm_pin, int enc_a_pin, int enc_b_pin);
>>>>>>> 2fb469a3443a680139b88f9acccccf10f1443cca

    int get_voltage() const;
    void move_motor(int voltage);
};

#endif // MOTOR_HPP