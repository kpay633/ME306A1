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

int nominal_speed = 200; // Default speed for motors
int approach_speed = 150; // Speed for approaching limit switches
int retreat_time = 180; // Time to retreat after hitting a limit switch
volatile uint16_t Plotter::motor_timer = 0;

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

void Plotter::MoveMotorTime(int voltage, Target target, uint16_t time){
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

void Plotter::MoveTo(int x_pos, int y_pos){

    float current_x = get_current_pos()[0];
    float current_y = get_current_pos()[1];
    float x_error = x_pos - current_x;
    float y_error = y_pos - current_y;
    float total_error = (abs(x_error) + abs(y_error)) / 2;
    int k_p = 5;
    int x_control_effort = x_error * k_p;
    int y_control_effort = y_error * k_p;
    int M1_control_effort = 100+(x_control_effort + y_control_effort) / 2;
    int M2_control_effort = 100+(x_control_effort - y_control_effort) / 2;



    while (total_error > 1){
        current_x = get_current_pos()[0];
        current_y = get_current_pos()[1];
        x_error = x_pos - current_x;
        y_error = y_pos - current_y;
        total_error = (abs(x_error) + abs(y_error)) / 2;
        x_control_effort = x_error * k_p;
        y_control_effort = y_error * k_p;
        M1_control_effort = 100 + (x_control_effort + y_control_effort) / 2;
        M2_control_effort = 100 + (x_control_effort - y_control_effort) / 2;
        Serial.print(current_x);
        Serial.print(" ");
        Serial.println(current_y);
        if(M1_control_effort > 200){
            M1_control_effort = 200;
        } else if (M1_control_effort < 200){
            M1_control_effort = -200;
        }
        if(M2_control_effort > 200){
            M2_control_effort = 200;
        } else if (M2_control_effort < 200){
            M2_control_effort = -200;
        }
        motor_A.move_motor(MotorID::M1, M1_control_effort, (M1_control_effort < 0) ? Direction::CW : Direction::CCW);
        motor_B.move_motor(MotorID::M2, M2_control_effort, (M2_control_effort < 0) ? Direction::CW : Direction::CCW);
    }
    StopMotors();
    Serial.print("stopped");

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
    Serial.print("testing");

    MoveMotors(nominal_speed, Target::LEFT);

    delay(3000);

    StopMotors();

    // MoveMotors(nominal_speed, Target::LEFT);
    // delay(1000);
    // MoveMotors(nominal_speed, Target::RIGHT);
    // delay(1000);
    // MoveMotors(nominal_speed, Target::UP);
    // delay(1000);
    // MoveMotors(nominal_speed, Target::DOWN);

    // MoveMotorTime(nominal_speed, Target::LEFT, 1000);
    // delay(1000);
    // MoveMotorTime(nominal_speed, Target::RIGHT, 1000);
    // delay(1000);
    // MoveMotorTime(nominal_speed, Target::UP, 1000);
    // delay(1000);
    // MoveMotorTime(nominal_speed, Target::DOWN, 1000);
    
}

void Plotter::home() {
    ResetEncoders();

    // ------------ HOME LEFT ------------ //
    while(!(Limit_Switch::switch_state & (1 << 0))){
        Serial.println("moving 1");
        MoveMotors(nominal_speed, Target::LEFT);
    }
    Serial.print("stopping 1");
    StopMotors();
    delay(1000);

    //Retreat right
    MoveMotorTime(approach_speed, Target::RIGHT, 2000);
    Limit_Switch::switch_state = 0;
        delay(1000);


    //Approach left
    while(!(Limit_Switch::switch_state & (1 << 0))){
        MoveMotors(approach_speed, Target::LEFT);
    }
    

    StopMotors();
    ResetEncoders();
    set_left_boundary(0.0);

    delay(1000);


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

<<<<<<< HEAD
void Plotter::MoveTo(int x_pos, int y_pos){

    float current_x = get_current_pos()[0];
    float current_y = get_current_pos()[1];
    float x_error = x_pos - current_x;
    float y_error = y_pos - current_y;
    float total_error = (abs(x_error) + abs(y_error)) / 2;
    int k_p = 1;
    int x_control_effort = x_error * k_p;
    int y_control_effort = y_error * k_p;
    int M1_control_effort = (x_control_effort + y_control_effort) / 2;
    int M2_control_effort = (x_control_effort - y_control_effort) / 2;


    while (total_error > 1){
        current_x = get_current_pos()[0];
        current_y = get_current_pos()[1];
        x_error = x_pos - current_x;
        y_error = y_pos - current_y;
        total_error = (abs(x_error) + abs(y_error)) / 2;
        x_control_effort = x_error * k_p;
        y_control_effort = y_error * k_p;
        M1_control_effort = 50 + (x_control_effort + y_control_effort) / 2;
        M2_control_effort = 50 + (x_control_effort - y_control_effort) / 2;
        
        motor_A.move_motor(MotorID::M1, M1_control_effort, (M1_control_effort < 0) ? Direction::CCW : Direction::CW);
        motor_B.move_motor(MotorID::M1, M2_control_effort, (M2_control_effort < 0) ? Direction::CCW : Direction::CW);

    }
    StopMotors();

}

ISR(INT2_vect){ 
    // motor_A.DisableMotor(); 
    Limit_Switch::switch_state |= (1 << 0);
}
ISR(INT3_vect){ 
    // motor_A.DisableMotor(); 
        Limit_Switch::switch_state |= (1 << 1);

}
// ISR(INT2_vect){ 
    // motor_A.DisableMotor(); 
//     Limit_Switch::switch_state |= (1 << 2);

// }
// ISR(INT3_vect){ 
    // motor_A.DisableMotor();
//     Limit_Switch::switch_state |= (1 << 3);

// }

ISR(TIMER2_OVF_vect) {
  Plotter::IncrementMotorTimer();
=======
void Plotter::move_to_target(float x, float y, int speed) {
    // Implement the logic to move the plotter to the target position (x, y) at the specified speed
    // This will involve calculating the necessary motor movements and updating current_pos accordingly
    // For now, we will just set the target position
    current_pos[0] = 0.0;
    current_pos[1] = 0.0;

    target_pos[0] = x;
    target_pos[1] = y;

    motor_A.ResetEncoder();
    motor_B.ResetEncoder();

    while (true){
        calc_pos_error(current_pos, target_pos);

        // Check if the error is within a small threshold
        if (std::abs(delta_pos[0]) < 0.01 && std::abs(delta_pos[1]) < 0.01) {
            break; // Target reached
        }

        while (abs(delta_pos[0]) > 0.01) {
            if (delta_pos[0] > 0) {
            motor_A.move_motor(MotorID::M1, speed, Direction::CCW);
            motor_B.move_motor(MotorID::M2, speed, Direction::CCW);
            } else {
            motor_A.move_motor(MotorID::M1, speed, Direction::CW);
            motor_B.move_motor(MotorID::M2, speed, Direction::CW);
            }
            current_pos[0] += (motor_A.GetEncoderDist() + motor_B.GetEncoderDist()) / 2; // Update current position based on motor movement
            calc_pos_error(current_pos, target_pos);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        motor_A.stop_motor(MotorID::M1);
        motor_B.stop_motor(MotorID::M2);

        while (abs(delta_pos[1]) > 0.01) {
            if (delta_pos[1] > 0) {
                motor_A.move_motor(MotorID::M1, speed, Direction::CCW);
                motor_B.move_motor(MotorID::M2, speed, Direction::CW);
            } else {
                motor_A.move_motor(MotorID::M1, speed, Direction::CW);
                motor_B.move_motor(MotorID::M2, speed, Direction::CCW);
            }
            current_pos[1] += (motor_A.GetEncoderDist() - motor_B.GetEncoderDist()) / 2; // Update current position based on motor movement
            calc_pos_error(current_pos, target_pos);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        motor_A.stop_motor(MotorID::M1);
        motor_B.stop_motor(MotorID::M2);
    }

    // After reaching the target position, update the current position
    current_pos[0] = target_pos[0];
    current_pos[1] = target_pos[1];
    std::cout << "Moved to target position: (" << current_pos[0] << ", " << current_pos[1] << ")\n";
>>>>>>> 2fb469a3443a680139b88f9acccccf10f1443cca
}