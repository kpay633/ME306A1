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

  public:
    Motor(int voltage, Timer timer, int pwm_pin, int enc_a_pin, int enc_b_pin);

    int get_voltage() const;
    void move_motor(int voltage);
};

#endif // MOTOR_HPP
