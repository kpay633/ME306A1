// LATEST WORKING VERSION

#include "plotter.h"
#include "limit_switch.hpp"
#include "motor.hpp"

#define MOT1_PWM_PIN PB1
#define MOT2_PWM_PIN PB2

#define MOT1_ENCA_PIN PD0
#define MOT1_ENCB_PIN PD1
#define MOT2_ENCA_PIN PD2
#define MOT2_ENCB_PIN PD3

Limit_Switch limit_switch_left(&DDRB, &PINB, &PORTB, PB7), limit_switch_right(&DDRB, &PINB, &PORTB, PB6), limit_switch_top(&DDRB, &PINB, &PORTB, PB4), limit_switch_bottom(&DDRB, &PINB, &PORTB, PB5);
// Motor motor_A(MotorID::M1), motor_B(MotorID::M2);

int nominal_speed = 200; // Default speed for motors
int approach_speed = 80; // Speed for approaching limit switches
int retreat_time = 100; // Time to retreat after hitting a limit switch

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
}

void Plotter::test(){
    motor_A->move_motor(MotorID::M1, 200, Direction::CW);
    motor_B->move_motor(MotorID::M2, 200, Direction::CW);

        while(1) {
      Serial.print("X ");
      Serial.print(get_current_pos()[0]);
      Serial.print(" Y ");
      Serial.println(get_current_pos()[1]);
  }

}

float *Plotter::get_current_pos() {
    float a = motor_A->GetEncoderDist() * 13.5 * 3.14 / 24.0 / 172.0;
    float b = motor_B->GetEncoderDist() * 13.5 * 3.14 / 24.0 / 172.0;
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
    motor_A->ResetEncoder();
    motor_B->ResetEncoder();

    while (!limit_switch_left.is_pressed()) {
        motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CW);
        motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CW);
    }

    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);

    // if (limit_switch_left.is_pressed()) {
    //     motor_A.stop_motor(MotorID::M1);
    //     motor_B.stop_motor(MotorID::M2);
    //     motor_A.move_motor(MotorID::M1, nominal_speed, Direction::CCW);
    //     motor_B.move_motor(MotorID::M2, nominal_speed, Direction::CCW);
    //     while (!limit_switch_left.is_pressed()) {
    //         motor_A.clockwise(approach_speed, 0);
    //         motor_B.clockwise(approach_speed, 0);
    //     }
    //     motor_A.stop_motor(MotorID::M1);
    //     motor_B.stop_motor(MotorID::M2);
    //     motor_A.ResetEncoder();
    //     motor_B.ResetEncoder();
    //     set_left_boundary(0.0);

    //     motor_A.anticlockwise(nominal_speed, retreat_time);
    //     motor_B.anticlockwise(nominal_speed, retreat_time);
    // }

    while (!limit_switch_right.is_pressed()) {
        motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CCW);
        motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CCW);
    }

    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);

    // if (limit_switch_right.is_pressed()) {
    //     motor_A.stop_motor(MotorID::M1);
    //     motor_B.stop_motor(MotorID::M2);
    //     motor_A.clockwise(nominal_speed, retreat_time);
    //     motor_B.clockwise(nominal_speed, retreat_time);
    //     while (!limit_switch_right.is_pressed()) {
    //         motor_A.anticlockwise(approach_speed, 0);
    //         motor_B.anticlockwise(approach_speed, 0);
    //     }
    //     motor_A.stop_motor(MotorID::M1);
    //     motor_B.stop_motor(MotorID::M2);

    //     set_right_boundary((motor_A.GetEncoderDist() + motor_B.GetEncoderDist()) / 2);

    //     motor_A.clockwise(nominal_speed, retreat_time);
    //     motor_B.clockwise(nominal_speed, retreat_time);
    // }

    while (!limit_switch_bottom.is_pressed()) {
        motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CW);
        motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CCW);
    }

    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);

    // if (limit_switch_bottom.is_pressed()) {
    //     motor_A.stop_motor(MotorID::M1);
    //     motor_B.stop_motor(MotorID::M2);
    //     motor_A.anticlockwise(nominal_speed, retreat_time);
    //     motor_B.clockwise(nominal_speed, retreat_time);
    //     while (!limit_switch_bottom.is_pressed()) {
    //         motor_A.clockwise(approach_speed, 0);
    //         motor_B.anticlockwise(approach_speed, 0);
    //     }
    //     motor_A.stop_motor(MotorID::M1);
    //     motor_B.stop_motor(MotorID::M2);

    //     motor_A.ResetEncoder();
    //     motor_B.ResetEncoder();

    //     set_bottom_boundary(0.0);

    //     motor_A.anticlockwise(nominal_speed, retreat_time);
    //     motor_B.clockwise(nominal_speed, retreat_time);
    // }

    while (!limit_switch_top.is_pressed()) {
        motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CCW);
        motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CW);
    }

    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);

    // if (limit_switch_top.is_pressed()) {
    //     motor_A.stop_motor(MotorID::M1);
    //     motor_B.stop_motor(MotorID::M2);
    //     motor_A.clockwise(nominal_speed, retreat_time);
    //     motor_B.anticlockwise(nominal_speed, retreat_time);
    //     while (!limit_switch_top.is_pressed()) {
    //         motor_A.anticlockwise(approach_speed, 0);
    //         motor_B.clockwise(approach_speed, 0);
    //     }
    //     motor_A.stop_motor(MotorID::M1);
    //     motor_B.stop_motor(MotorID::M2);

    //     set_top_boundary((motor_A.GetEncoderDist() - motor_B.GetEncoderDist()) / 2);

    //     motor_A.clockwise(nominal_speed, retreat_time);
    //     motor_B.anticlockwise(nominal_speed, retreat_time);
    // }

    // while (!limit_switch_bottom.is_pressed()) {
    //     motor_A.move_motor(MotorID::M1, approach_speed, Direction::CW);
    //     motor_B.move_motor(MotorID::M2, approach_speed, Direction::CCW);
    // }
    // motor_A.stop_motor(MotorID::M1);
    // motor_B.stop_motor(MotorID::M2);

    // while (!limit_switch_left.is_pressed()) {
    //     motor_A.move_motor(MotorID::M1, approach_speed, Direction::CW);
    //     motor_B.move_motor(MotorID::M2, approach_speed, Direction::CW);
    // }
    // motor_A.stop_motor(MotorID::M1);
    // motor_B.stop_motor(MotorID::M2);

    current_pos[0] = get_left_boundary();
    current_pos[1] = get_bottom_boundary();
}

void Plotter::move_to_target(float x_target, float y_target, float speed) {
    Serial.println("=== Simple move_to_target ===");
    
    const float POSITION_TOLERANCE = 0.1;
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
        Serial.print(") | Target: ("); Serial.print(x_target);
        Serial.print(", "); Serial.print(y_target);
        Serial.print(") | Error: ("); Serial.print(delta_x, 4);
        Serial.print(", "); Serial.print(delta_y, 4);
        

        // Check if we're close enough
        if (fabs(delta_x) < POSITION_TOLERANCE && fabs(delta_y) < POSITION_TOLERANCE) {
            Serial.println("Position reached!");
            break;
        }

        // Convert X/Y error to motor movements
        float motorA_move = delta_x + delta_y;
        float motorB_move = delta_x - delta_y;

        Serial.print("Motor moves - A: "); Serial.print(motorA_move, 4);
        Serial.print(", B: "); Serial.println(motorB_move, 4);

        // Move motors (proportional control would be better here)
        Direction dir_A = (motorA_move >= 0) ? Direction::CCW : Direction::CW;
        Direction dir_B = (motorB_move >= 0) ? Direction::CCW : Direction::CW;

        motor_A->move_motor(MotorID::M1, speed, dir_A);
        motor_B->move_motor(MotorID::M2, speed, dir_B);

        delay(100); // Increased delay to see changes better
        
        // Stop motors after each iteration to see if position changed
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        delay(50);
    }
    
    if (iteration >= MAX_ITERATIONS) {
        Serial.println("Max iterations reached - stopping!");
    }

    // Stop motors
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);
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