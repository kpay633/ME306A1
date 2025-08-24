// LATEST WORKING VERSION

#include "plotter.h"
#include "limit_switch.hpp"
#include "motor.hpp"
#include "controller.hpp"

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

// Motor motor_A(MotorID::M1), motor_B(MotorID::M2);

int nominal_speed = 200; // Default speed for motors
int approach_speed = 160; // Speed for approaching limit switches
int retreat_time = 1000; // Time to retreat after hitting a limit switch
int retreat_speed = 180;

Limit_Switch limitSwitch;

Plotter::Plotter(Motor* Motor_A, Motor* Motor_B) : motor_A(Motor_A), motor_B(Motor_B) {
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
    time = 0;
    motor_A->ResetEncoder();
    motor_B->ResetEncoder();
    timer2_init();

}

void Plotter::timer2_init() {
  TCCR2A = 0;              // normal mode
  TCCR2B = _BV(CS22);      // prescaler = 64
  TIMSK2 = _BV(TOIE2);     // enable overflow interrupt
}

void Plotter::test(){
    motor_A->move_motor(MotorID::M1, 200, Direction::CW);
    motor_B->move_motor(MotorID::M2, 200, Direction::CW);



    //     while(1) {
    //   Serial.print("X ");
    //   Serial.print(get_current_pos()[0]);
    //   Serial.print(" Y ");
    //   Serial.println(get_current_pos()[1]);
//   }

}

void Plotter::MoveTo(){
   motor_A->ResetEncoder();
   motor_B->ResetEncoder();

   float error = 42.39 - motor_A->GetEncoderDist();
   int k_p = 0.5;
   int control_effort = int(error * k_p);
   Direction direction = Direction::CW;
   
   while(abs(error) > 1){
    error = 42.39 - motor_A->GetEncoderDist();
    control_effort = int(abs(error) * k_p + 140);
    if (control_effort > 255){
        control_effort = 255;
    }
    if (error < 0){
        direction = Direction::CW;
    } else {
        direction = Direction::CCW;
    }
    motor_A->move_motor(MotorID::M1, (control_effort), direction);
    Serial.print("ENC A ");
    Serial.print(motor_A->GetEncoderDist());
    Serial.print(" ERROR = ");
    Serial.println(error);

   }
   motor_A->stop_motor(MotorID::M1);
   motor_B->stop_motor(MotorID::M2);
}

void Plotter::MoveTime(int move_time, Target target, int speed){
    TCNT2 = 0;
    time = 0;

    while(time < move_time){
        switch (target) {
        case Target::Left:
            motor_A->move_motor(MotorID::M1, speed, Direction::CW);
            motor_B->move_motor(MotorID::M2, speed, Direction::CCW);
            break;
        case Target::Right:
            motor_A->move_motor(MotorID::M1, speed, Direction::CCW);
            motor_B->move_motor(MotorID::M2, speed, Direction::CW);
            break;
        case Target::Up:
            motor_A->move_motor(MotorID::M1, speed, Direction::CCW); 
            motor_B->move_motor(MotorID::M2, speed, Direction::CCW); 
            break;
        case Target::Down:
            motor_A->move_motor(MotorID::M1, speed, Direction::CW); 
            motor_B->move_motor(MotorID::M2, speed, Direction::CW); 
            break;
        case Target::None:
            break;
        }
    }
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);
}

void Plotter::move_to_target(float x_target, float y_target, float speed) {
    Serial.println("=== Simple move_to_target with Controller class ===");
    Serial.print("Motors disabled = ");
    Serial.println(motor_A->IsDisabled());

    if ((x_target > get_right_boundary()) || (x_target < 0) || (y_target > get_top_boundary()) || (y_target < 0)){
        Serial.println("ERROR - OUTSIDE BOUNDS");
        return;
    }
    Controller ctrl(4.0, 0.0, 0.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0);

    // int kp_x = 4;
    // int kp_y = 4;
    float motorA_move;
    float motorB_move;
    float max_abs;
    float scale;

    const float POSITION_TOLERANCE = 1;
    int iteration = 0;
    const int MAX_ITERATIONS = 100; // Prevent infinite loop

    while (iteration < MAX_ITERATIONS) {
        iteration++;
        
        // Get current X/Y position
        float* current = get_current_pos();
        float delta_x = x_target - current[0];
        float delta_y = y_target - current[1];

        Serial.print("Iter "); Serial.print(iteration);
        Serial.print(" | Current: ("); Serial.print(current[0], 4);
        Serial.print(", "); Serial.print(current[1], 4);
        Serial.print(") | Counts: (MA "); Serial.print(motor_A->GetEncoderCounts());
        Serial.print("MB "); Serial.print(motor_B->GetEncoderCounts());
        Serial.print(") | Target: ("); Serial.print(x_target);
        Serial.print(", "); Serial.print(y_target);
        Serial.print(") | Error: ("); Serial.print(delta_x, 4);
        Serial.print(", "); Serial.print(delta_y, 4);
        

        // Check if we're close enough
        if (abs(delta_x) < POSITION_TOLERANCE && abs(delta_y) < POSITION_TOLERANCE) {
            Serial.println("Position reached!");
            break;
        }

        // control_effort_X = delta_x * kp_x;
        // control_effort_Y = delta_y * kp_y;
        ctrl.calculateControlEffort(delta_x, delta_y, 0.0, 0.0, controlMode::P);
        
        motorA_move = ctrl.getMotorLeftControlEffort();
        motorB_move = ctrl.getMotorRightControlEffort();
        // Convert X/Y error to motor movements

        // Move motors (proportional control would be better here)
        Direction dir_A = (motorA_move >= 0) ? Direction::CCW : Direction::CW;
        Direction dir_B = (motorB_move >= 0) ? Direction::CW : Direction::CCW;

        motorA_move = abs(motorA_move);
        motorB_move = abs(motorB_move);

        if((motorA_move > 110) || (motorB_move > 110)){
            max_abs = (motorA_move > motorB_move) ? motorA_move : motorB_move;
            scale = 110 / max_abs;
            motorA_move = motorA_move * scale;
            motorB_move = motorB_move * scale;
        }

        Serial.print("Motor moves - A: "); Serial.print(motorA_move, 4);
        Serial.print(", B: "); Serial.println(motorB_move, 4);

        motor_A->move_motor(MotorID::M1, (motorA_move + 140), dir_A);
        motor_B->move_motor(MotorID::M2, (motorB_move + 140), dir_B);

        }
    
    if (iteration >= MAX_ITERATIONS) {
        Serial.println("Max iterations reached - stopping!");
    }

    // Stop motors
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);
}

float *Plotter::get_current_pos() {
    float a = motor_A->GetEncoderDist();
    float b = motor_B->GetEncoderDist();
    current_pos[0] = (a + b) / 2.0;
    current_pos[1] = (a - b) / 2.0;
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

void Plotter::home() {
    
    Serial.println("=== HOMING START ===");

    motor_A->EnableMotor();
    motor_B->EnableMotor();

    if (limitSwitch.is_pressed(SWLEFT)){
        allowed_switch = Target::Left;
        Serial.println("trying to move Right");
        MoveTime(retreat_time, Target::Right, retreat_speed);
        Serial.println("moved Right");
    }
    if (limitSwitch.is_pressed(SWRIGHT)){
        allowed_switch = Target::Right;
        Serial.println("trying to move Left");
        MoveTime(retreat_time, Target::Left, retreat_speed);
        Serial.println("moved left");
    }
    if (limitSwitch.is_pressed(SWTOP)){
        allowed_switch = Target::Up;
        Serial.println("trying to move Down");
        MoveTime(retreat_time, Target::Down, retreat_speed);
        Serial.println("Moved down");
    }
    if (limitSwitch.is_pressed(SWBOTTOM)){
        allowed_switch = Target::Down;
        Serial.println("trying to move Up");
        MoveTime(retreat_time, Target::Up, retreat_speed);
        Serial.println("Moved up");
    }

    // --- LEFT (X min) ---
    allowed_switch = Target::Left;
    allowed_switch2 = Target::None;
    while (!limitSwitch.is_pressed(SWLEFT)) {
        motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CW); 
        motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CCW);
    }
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);
    
    // Back off
    MoveTime(retreat_time, Target::Right, retreat_speed);
    // delay(50);

    // while (!limitSwitch.is_pressed(SWLEFT)) {
    //     motor_A->move_motor(MotorID::M1, approach_speed, Direction::CW); 
    //     motor_B->move_motor(MotorID::M2, approach_speed, Direction::CCW);
    // }
    // motor_A->stop_motor(MotorID::M1);
    // motor_B->stop_motor(MotorID::M2);
    // set_left_boundary(0);

    // motor_A->ResetEncoder();
    // motor_B->ResetEncoder();

    // // Back off
    // MoveTime(retreat_time, Target::Right, retreat_speed);
    // // delay(50);

        // --- BOTTOM (Y min) ---
    allowed_switch = Target::Down;
    while (!limitSwitch.is_pressed(SWBOTTOM)) {
        motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CW); 
        motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CW);
    }
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);

    // Back off
    MoveTime(retreat_time, Target::Up, retreat_speed);
    // delay(50);

    while (!limitSwitch.is_pressed(SWBOTTOM)) {
        motor_A->move_motor(MotorID::M1, approach_speed, Direction::CW); 
        motor_B->move_motor(MotorID::M2, approach_speed, Direction::CW);
    }
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);
    set_bottom_boundary(0);

    // // Back off
    // MoveTime(500, Target::Up, retreat_speed);
    // // delay(50);

    // --- APPROACH LEFT AGAIN ---
    allowed_switch2 = Target::Left;
    while (!limitSwitch.is_pressed(SWLEFT)) {
        motor_A->move_motor(MotorID::M1, approach_speed, Direction::CW); 
        motor_B->move_motor(MotorID::M2, approach_speed, Direction::CCW);
    }

    // // --- APPROACH BOTTOM AGAIN ---
    // while (!limitSwitch.is_pressed(SWBOTTOM)) {
    //     motor_A->move_motor(MotorID::M1, approach_speed, Direction::CW); 
    //     motor_B->move_motor(MotorID::M2, approach_speed, Direction::CW);
    // }


    MoveTime(200, Target::Up, retreat_speed);
    MoveTime(200, Target::Right, retreat_speed);

    motor_A->ResetEncoder();
    motor_B->ResetEncoder();
    
    Serial.print("reset encoders");
    
    // --- BACK OFF ---
    MoveTime(retreat_time, Target::Right, retreat_speed);
    MoveTime(retreat_time, Target::Up, retreat_speed);

    // --- RIGHT (X max) ---
    allowed_switch = Target::Right;
    allowed_switch2 = Target::None;

    while (!limitSwitch.is_pressed(SWRIGHT)) {
        motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CCW); 
        motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CW);
    }
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);

    // Back off
    MoveTime(retreat_time, Target::Left, retreat_speed);
    // delay(50);

    while (!limitSwitch.is_pressed(SWRIGHT)) {
        motor_A->move_motor(MotorID::M1, approach_speed, Direction::CCW); 
        motor_B->move_motor(MotorID::M2, approach_speed, Direction::CW);
    }
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);
    float right_boundary = (get_current_pos()[0]);
    set_right_boundary(right_boundary);

    // Back off
    MoveTime(2000, Target::Left, retreat_speed);
    // delay(50);

    // --- TOP (Y max) ---
    allowed_switch = Target::Up;

    while (!limitSwitch.is_pressed(SWTOP)) {
        motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CCW); 
        motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CCW);
    }
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);

    // Back off
    MoveTime(retreat_time, Target::Down, retreat_speed);
    // delay(50);

    while (!limitSwitch.is_pressed(SWTOP)) {
        motor_A->move_motor(MotorID::M1, approach_speed, Direction::CCW); 
        motor_B->move_motor(MotorID::M2, approach_speed, Direction::CCW);
    }
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);
    float top_boundary = (get_current_pos()[1]);
    set_top_boundary(top_boundary);

    // Back off
    MoveTime(2000, Target::Down, retreat_speed);
    // delay(50);

    allowed_switch = Target::None;
    allowed_switch2 = Target::None;

    // --- Return to bottom-left (home) ---
    // move_to_target(0, 0, nominal_speed);

    Serial.println("=== HOMING COMPLETE ===");


    // while (!limitSwitch.is_pressed(SWLEFT))
    // {
    //     Serial.print("X ");
    //   Serial.print(get_current_pos()[0]);
    //   Serial.print(" Y ");
    //   Serial.println(get_current_pos()[1]);      
    //   motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CW); 
    // motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CCW);
    // }
    
    // set_left_boundary(0);
    // motor_A->stop_motor(MotorID::M1);
    // motor_B->stop_motor(MotorID::M2);

    // while (!limitSwitch.is_pressed(SWBOTTOM))
    // {
    //   Serial.print("X ");
    //   Serial.print(get_current_pos()[0]);
    //   Serial.print(" Y ");
    //   Serial.println(get_current_pos()[1]); 
    //     motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CW); 
    //     motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CW); 
    // }
    // set_bottom_boundary(0);
    // motor_A->stop_motor(MotorID::M1);
    // motor_B->stop_motor(MotorID::M2);

    // motor_A->ResetEncoder();
    // motor_B->ResetEncoder();

    // while (!limitSwitch.is_pressed(SWRIGHT))
    // {
    //     Serial.print("X ");
    //   Serial.print(get_current_pos()[0]);
    //   Serial.print(" Y ");
    //   Serial.println(get_current_pos()[1]); 
    //     motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CCW); 
    //     motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CW); 
    // }

    // set_right_boundary(get_current_pos()[0]);
    // motor_A->stop_motor(MotorID::M1);
    // motor_B->stop_motor(MotorID::M2);

    // while (!limitSwitch.is_pressed(SWTOP))
    // {
    //     Serial.print("X ");
    //   Serial.print(get_current_pos()[0]);
    //   Serial.print(" Y ");
    //   Serial.println(get_current_pos()[1]); 
    //     motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CCW); 
    //     motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CCW); 
    // }

    // set_top_boundary(get_current_pos()[1]);
    // motor_A->stop_motor(MotorID::M1);
    // motor_B->stop_motor(MotorID::M2);
    
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

void Plotter::IncrementTime(){
    time++;
}

Target Plotter::GetAllowedSwitch1(){
    return allowed_switch;
}

Target Plotter::GetAllowedSwitch2(){
    return allowed_switch2;
}