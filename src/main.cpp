// LATEST WORKING VERSION

#include <avr/io.h>
#include <Arduino.h>

#include "motor.hpp"
#include "plotter.h"
#include "limit_switch.hpp"

#define MOT1_PWM_PIN PB1
#define MOT2_PWM_PIN PB2

#define MOT1_ENCA_PIN PD0
#define MOT1_ENCB_PIN PD1
#define MOT2_ENCA_PIN PD2
#define MOT2_ENCB_PIN PD3

#define SWTOP PD2
#define SWBOTTOM PD3
#define SWRIGHT PE4
#define SWLEFT PE5


// void new_state(State);
// void doIdle(Plotter&);
// void doHoming(Plotter&);
// void doMoving(Plotter&);
// void doFault(Plotter&);

enum State {
    IDLE,
    HOMING,
    MOVING,
    FAULT
};

State state = IDLE;

Motor* motor1;
Motor* motor2;

int main() {
    cli();
    motor1 = new Motor(MotorID::M1);  // voltage=0, timer=1, pwm_pin=PB1, enc_a=PD2, enc_b=PD3
    motor2 = new Motor(MotorID::M2);  // another motor on Timer2 with different encoder pins
    Plotter plotter(motor1, motor2);
    Serial.begin(9600);
    sei();
    plotter.home();
    plotter.move_to_target(100, 100, 160);
    plotter.move_to_target(10, 10, 160);
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

ISR(INT2_vect){
    if (state != State::HOMING){
    }
}

ISR(INT3_vect){
    if (state != State::HOMING){
    }
}

ISR(INT4_vect){
    if (state != State::HOMING){
    }
}

ISR(INT5_vect){
    if (state != State::HOMING){
    }
}