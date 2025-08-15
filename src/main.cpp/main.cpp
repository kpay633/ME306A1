#include <iostream>
#include <chrono>
#include <thread>

#include "motor.hpp"

#define MOT1_PWM_PIN PB1
#define MOT2_PWM_PIN PB2

#define MOT1_ENCA_PIN PD1
#define MOT1_ENCB_PIN PD2
#define MOT2_ENCA_PIN PD3
#define MOT2_ENCB_PIN PD4

enum State {
    IDLE,
    HOMING,
    MOVING,
    FAULT
};
struct GCode {
    
};

void new_state(State);
void GCodeParser(*GCode);
void doIdle(Plotter&);
void doHoming(Plotter&);
void doMoving(Plotter&);
void doFault(Plotter&);
State state = IDLE;

int main() {
    Plotter plotter;
    Motor motor1(0, 1, MOT1_PWM_PIN, MOT1_ENCA_PIN, MOT1_ENCB_PIN);  // voltage=0, timer=1, pwm_pin=PB1, enc_a=PD2, enc_b=PD3
    Motor motor2(0, 2, MOT2_PWM_PIN, MOT2_ENCA_PIN, MOT2_ENCB_PIN);  // another motor on Timer2 with different encoder pins
    char key_in;
    int x;
    int y;
    
    while(1) {
        key_in = getchar();
        g_code_extractor(key_in);
        switch(state) {
            case IDLE:
                if(key_in == *(G1)) new_state(HOMING);
                if(key_in == *(G28)) new_state(MOVING);
                doIdle(plotter);
                break;
            case HOMING:
                doHoming(plotter);
                break;
            case MOVING:
                doMoving(plotter);  // <--- motor control loop here
                break;
            case FAULT:
                doFault(plotter);
                break;
        }
        // delay ???
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}

void new_state(State s) {
    state = s;
    std::cout << "Transitioning to state: " << s << "\n";
}
void doIdle(Plotter& plotter) {
    std::cout << "Idle ..." << "\n";
}
void doHoming(Plotter& plotter) {
    plotter.home();
    new_state(IDLE);
}
void doMoving(Plotter& plotter);
void doFault(Plotter& plotter);