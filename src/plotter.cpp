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

Limit_Switch limit_switch_left(5), limit_switch_right(4), limit_switch_top(2), limit_switch_bottom(3);
Motor motor_A(0, MotorID::M1, MOT1_PWM_PIN, MOT1_ENCA_PIN, MOT1_ENCB_PIN), motor_B(0, MotorID::M2, MOT2_PWM_PIN, MOT2_ENCA_PIN, MOT2_ENCB_PIN);

int nominal_speed = 200; // Default speed for motors
int approach_speed = 80; // Speed for approaching limit switches
int retreat_time = 100; // Time to retreat after hitting a limit switch

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
}

float *Plotter::get_current_pos() {
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
    motor_A.ResetEncoder();
    motor_B.ResetEncoder();

    while (!limit_switch_left.is_pressed()) {
        motor_A.move_motor(MotorID::M1, nominal_speed, Direction::CW);
        motor_B.move_motor(MotorID::M2, nominal_speed, Direction::CW);
    }

    motor_A.stop_motor(MotorID::M1);
    motor_B.stop_motor(MotorID::M2);

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
        motor_A.move_motor(MotorID::M1, nominal_speed, Direction::CCW);
        motor_B.move_motor(MotorID::M2, nominal_speed, Direction::CCW);
    }

    motor_A.stop_motor(MotorID::M1);
    motor_B.stop_motor(MotorID::M2);

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
        motor_A.move_motor(MotorID::M1, nominal_speed, Direction::CW);
        motor_B.move_motor(MotorID::M2, nominal_speed, Direction::CCW);
    }

    motor_A.stop_motor(MotorID::M1);
    motor_B.stop_motor(MotorID::M2);

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
        motor_A.move_motor(MotorID::M1, nominal_speed, Direction::CCW);
        motor_B.move_motor(MotorID::M2, nominal_speed, Direction::CW);
    }

    motor_A.stop_motor(MotorID::M1);
    motor_B.stop_motor(MotorID::M2);

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