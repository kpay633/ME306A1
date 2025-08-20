// LATEST WORKING VERSION

#include <avr/io.h>
#include <Arduino.h>

#include "motor.hpp"
#include "plotter.h"

#define MOT1_PWM_PIN PB1
#define MOT2_PWM_PIN PB2

#define MOT1_ENCA_PIN PD0
#define MOT1_ENCB_PIN PD1
#define MOT2_ENCA_PIN PD2
#define MOT2_ENCB_PIN PD3

enum State {
    IDLE,
    HOMING,
    MOVING,
    FAULT
};


// void new_state(State);
// void doIdle(Plotter&);
// void doHoming(Plotter&);
// void doMoving(Plotter&);
// void doFault(Plotter&);
// State state = IDLE;

int main() {
    Plotter plotter;
    Serial.begin(9600);
    cli();
    Motor motor1(0, MotorID::M1, MOT1_PWM_PIN, MOT1_ENCA_PIN, MOT1_ENCB_PIN);  // voltage=0, timer=1, pwm_pin=PB1, enc_a=PD2, enc_b=PD3
    Motor motor2(0, MotorID::M2, MOT2_PWM_PIN, MOT2_ENCA_PIN, MOT2_ENCB_PIN);  // another motor on Timer2 with different encoder pins
    motor1.move_motor(MotorID::M1, 200, Direction::CW);
    motor2.move_motor(MotorID::M2, 200, Direction::CW);

    sei();
    
    while(1) {
      Serial.print("ENC A ");
      Serial.print(motor1.GetEncoderDist());
      Serial.print("ENC B ");
      Serial.println(motor2.GetEncoderDist());
  }
}

// void new_state(State s) {
//     state = s;
//     std::cout << "Transitioning to state: " << s << "\n";
// }
// void doIdle(Plotter& plotter) {
//     std::cout << "Idle ..." << "\n";
// }
// void doHoming(Plotter& plotter) {
//     plotter.home();
//     new_state(IDLE);
// }
// void doMoving(Plotter& plotter);
// void doFault(Plotter& plotter);