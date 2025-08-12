#ifndef MOTOR_HPP
#define MOTOR_HPP

enum class Timer {
  M1_TIMER3A,  // PE3 → OC3A → D5
  M2_TIMER4A   // PH3 → OC4A → D6
};

class Motor {
  private:
    int voltage = 0;

    Timer timer;
    int pwm_pin;
    int enc_a_pin;
    int enc_b_pin;

    volatile uint16_t encCount;

  public:
    Motor(int voltage, Timer timer, int pwm_pin, int enc_a_pin, int enc_b_pin);

    // int get_voltage() const;
    // int read_encoder(); 
    
    // void stop_motor(void);
    void move_motor(int voltage);


    // void fast_clockwise(int voltage, int ms);
    // void fast_anticlockwise(int voltage, int ms);

    int GetEncoderDist();
    void ResetEncoder();
    void incrementEncoder();

    static Motor* motor1;
    static Motor* motor2;

};

#endif // MOTOR_HPP