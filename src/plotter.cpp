#include "plotter.h"
#include "limit_switch.hpp"
#include "motor.hpp"

#define MOT1_PWM_PIN PB1
#define MOT2_PWM_PIN PB2

#define MOT1_ENCA_PIN PE4 //YELLOW
#define MOT1_ENCB_PIN PE0 //WHITE
#define MOT2_ENCA_PIN PE5 //YELLOW
#define MOT2_ENCB_PIN PE1 //WHITE

Limit_Switch limit_switch;
Motor motor_A(0, MotorID::M1, MOT1_PWM_PIN, MOT1_ENCA_PIN, MOT1_ENCB_PIN), motor_B(0, MotorID::M2, MOT2_PWM_PIN, MOT2_ENCA_PIN, MOT2_ENCB_PIN);

int nominal_speed = 255; // Default speed for motors
int approach_speed = 80; // Speed for approaching limit switches
int retreat_time = 100; // Time to retreat after hitting a limit switch
volatile uint32_t Plotter::motor_timer = 0;

Plotter::Plotter() {
    current_pos[0] = 0.0;
    current_pos[1] = 0.0;
    target_pos[0] = 0.0;
    target_pos[1] = 0.0;
    delta_pos[0] = 0.0;
    delta_pos[1] = 0.0;
    left_boundary = 0.0;
    right_boundary = 0.0;
    top_boundary = 0.0;
    bottom_boundary = 0.0;
    cli();
    TIMSK2 |= (1 << TOIE2); //Enable overflow interupts for timer 2
    TCCR2A = 0;  // Normal operation
    TCNT2 = 0;
    sei(); 
}

float *Plotter::get_current_pos() {
    float a = motor_A.GetEncoderDist() / 24.0 * 13.5 /172.0 * 3.14;
    float b = motor_B.GetEncoderDist() / 24.0 * 13.5 /172.0 * 3.14;
    current_pos[0] = (a + b) / 2;
    current_pos[1] = (a - b) / 2;
    return current_pos;
}

void Plotter::set_current_pos(float pos[2]) {
    current_pos[0] = pos[0];
    current_pos[1] = pos[1];
}

float *Plotter::get_target_pos() {
    return target_pos;
}

void Plotter::set_target_pos(float pos[2]) {
    target_pos[0] = pos[0];
    target_pos[1] = pos[1];
}

float *Plotter::calc_pos_error(float current_pos[2], float target_pos[2]) {
    delta_pos[0] = target_pos[0] - current_pos[0];
    delta_pos[1] = target_pos[1] - current_pos[1];
    return delta_pos;
}

void Plotter::MoveMotorTime(int voltage, Target target, int time){
    cli();
    TCCR2B |= (1 << CS21) | (1 << CS20); // Prescaler = 64 = 1ms overflow
    TCNT2 = 0;
    motor_timer = 0;
    sei();
    
    while(motor_timer < time){
        switch (target) {
            case Target::LEFT:
                motor_A.move_motor(MotorID::M1, voltage, Direction::CCW);
                motor_B.move_motor(MotorID::M2, voltage, Direction::CCW);
            break;
            case Target::RIGHT:
                motor_A.move_motor(MotorID::M1, voltage, Direction::CW);
                motor_B.move_motor(MotorID::M2, voltage, Direction::CW);
            break;
            case Target::UP:
                motor_A.move_motor(MotorID::M1, voltage, Direction::CCW);
                motor_B.move_motor(MotorID::M2, voltage, Direction::CW);
            break;
            case Target::DOWN:
                motor_A.move_motor(MotorID::M1, voltage, Direction::CW);
                motor_B.move_motor(MotorID::M2, voltage, Direction::CCW);
            break;
        }
    }
    motor_A.stop_motor(MotorID::M1);
    motor_B.stop_motor(MotorID::M2);
}

void Plotter::IncrementMotorTimer(){
    motor_timer++;
}

void Plotter::StopMotors(){
    motor_A.stop_motor(MotorID::M1);
    motor_B.stop_motor(MotorID::M2);
}

void Plotter::ResetEncoders(){
    motor_A.ResetEncoder();
    motor_B.ResetEncoder();
}

void Plotter::MoveMotors(int voltage, Target target){
    switch (target) {
        case Target::LEFT:
            motor_A.move_motor(MotorID::M1, voltage, Direction::CCW);
            motor_B.move_motor(MotorID::M2, voltage, Direction::CCW);
        break;
        case Target::RIGHT:
            motor_A.move_motor(MotorID::M1, voltage, Direction::CW);
            motor_B.move_motor(MotorID::M2, voltage, Direction::CW);
        break;
        case Target::UP:
            motor_A.move_motor(MotorID::M1, voltage, Direction::CCW);
            motor_B.move_motor(MotorID::M2, voltage, Direction::CW);
        break;
        case Target::DOWN:
            motor_A.move_motor(MotorID::M1, voltage, Direction::CW);
            motor_B.move_motor(MotorID::M2, voltage, Direction::CCW);
        break;
    }
}

void Plotter::test() {
    ResetEncoders();

    motor_A.move_motor(MotorID::M1, nominal_speed, Direction::CCW);
    motor_B.move_motor(MotorID::M2, nominal_speed, Direction::CCW);

    while(1){
        Serial.print("X pos = ");
        Serial.print(get_current_pos()[0]);
        Serial.print(" Y pos = ");
        Serial.println(get_current_pos()[1]);
        // Serial.print("Dist ");
        // Serial.println(motor_A.GetEncoderDist());
        // motor_A.GetEncoderDist();
    }

    
}

void Plotter::home() {
    ResetEncoders();

    // ------------ HOME LEFT ------------ //
    while(!(Limit_Switch::switch_state & (1 << 0))){
        MoveMotors(nominal_speed, Target::LEFT);
    }

    StopMotors();

    //Retreat right
    MoveMotorTime(approach_speed, Target::RIGHT, 2000);
    Limit_Switch::switch_state = 0;

    //Approach left
    while(!(Limit_Switch::switch_state & (1 << 0))){
        MoveMotors(approach_speed, Target::LEFT);
    }
    
    StopMotors();
    ResetEncoders();
    set_left_boundary(0.0);

    //Retreat right
    MoveMotorTime(approach_speed, Target::RIGHT, 2000);
    Limit_Switch::switch_state = 0;

    // ------------ HOME BOTTOM ------------ //
    while(!(Limit_Switch::switch_state & (1 << 1))){
        MoveMotors(nominal_speed, Target::DOWN);
    }

    StopMotors();

    //Retreat up
    MoveMotorTime(approach_speed, Target::UP, 2000);
    Limit_Switch::switch_state = 0;

    //Approach bottom
    while(!(Limit_Switch::switch_state & (1 << 1))){
        MoveMotors(approach_speed, Target::DOWN);
    }
    
    StopMotors();
    
    set_left_boundary(0.0);

    //Retreat right
    MoveMotorTime(approach_speed, Target::UP, 2000);
    Limit_Switch::switch_state = 0;

    // ------------ HOME RIGHT ------------ //
    while(!(Limit_Switch::switch_state & (1 << 0))){
        MoveMotors(nominal_speed, Target::RIGHT);
    }

    StopMotors();

    //Retreat left
    MoveMotorTime(approach_speed, Target::LEFT, 2000);
    Limit_Switch::switch_state = 0;

    //Approach right
    while(!(Limit_Switch::switch_state & (1 << 0))){
        MoveMotors(approach_speed, Target::RIGHT);
    }
    StopMotors();
    
    set_right_boundary((motor_A.GetEncoderDist() + motor_B.GetEncoderDist()) / 2);

    //Retreat left
    MoveMotorTime(approach_speed, Target::LEFT, 3000);
    Limit_Switch::switch_state = 0;

    // ------------ HOME TOP ------------ //
    while(!(Limit_Switch::switch_state & (1 << 1))){
        MoveMotors(nominal_speed, Target::UP);
    }

    StopMotors();

    //Retreat left
    MoveMotorTime(approach_speed, Target::LEFT, 2000);
    Limit_Switch::switch_state = 0;

    //Approach right
    while(!(Limit_Switch::switch_state & (1 << 0))){
        MoveMotors(nominal_speed, Target::RIGHT);
    }
    StopMotors();
    
    set_right_boundary((motor_A.GetEncoderDist() + motor_B.GetEncoderDist()) / 2);

    //Retreat left
    MoveMotorTime(approach_speed, Target::LEFT, 2000);
    Limit_Switch::switch_state = 0;

    Serial.println("finished homing");
}

float Plotter::get_left_boundary() {
    return left_boundary;
}

float Plotter::get_right_boundary() {
    return right_boundary;
}

float Plotter::get_top_boundary() {
    return top_boundary;
}

float Plotter::get_bottom_boundary() {
    return bottom_boundary;
}

void Plotter::set_left_boundary(float boundary) {
    left_boundary = boundary;
}

void Plotter::set_right_boundary(float boundary) {
    right_boundary = boundary;
}

void Plotter::set_top_boundary(float boundary) {
    top_boundary = boundary;
}

void Plotter::set_bottom_boundary(float boundary) {
    bottom_boundary = boundary;
}

ISR(INT0_vect){ 
    // motor_A.DisableMotor(); 
    Limit_Switch::switch_state |= (1 << 0);
}
ISR(INT1_vect){ 
    // motor_A.DisableMotor(); 
        Limit_Switch::switch_state |= (1 << 1);

}
// ISR(INT2_vect){ 
//     // motor_A.DisableMotor(); 
//     Limit_Switch::switch_state |= (1 << 2);

// }
// ISR(INT3_vect){ 
//     // motor_A.DisableMotor();
//     Limit_Switch::switch_state |= (1 << 3);

// }

ISR(TIMER2_OVF_vect) {
  Plotter::IncrementMotorTimer();
}