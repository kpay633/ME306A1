#include "plotter.h"
#include "limit_switch.hpp"
#include "motor.h"
#include "encoder.h"

Limit_Switch limit_switch_left(&DDRB, &PINB, &PORTB, PB7), limit_switch_right(&DDRB, &PINB, &PORTB, PB6), limit_switch_top(&DDRB, &PINB, &PORTB, PB4), limit_switch_bottom(&DDRB, &PINB, &PORTB, PB5);
Motor motor_A, motor_B;
Encoder encoder_A, encoder_B;

Plotter::Plotter() {
    current_pos[0] = 0;
    current_pos[1] = 0;
    target_pos[0] = 0;
    target_pos[1] = 0;
    delta_pos[0] = 0;
    delta_pos[1] = 0;
}

int *Plotter::get_current_pos() {
    return current_pos;
}

void Plotter::set_current_pos(int pos[2]) {
    current_pos[0] = pos[0];
    current_pos[1] = pos[1];
}

int *Plotter::get_target_pos() {
    return target_pos;
}

void Plotter::set_target_pos(int pos[2]) {
    target_pos[0] = pos[0];
    target_pos[1] = pos[1];
}

int *Plotter::calc_pos_error(int current_pos[2], int target_pos[2]) {
    delta_pos[0] = target_pos[0] - current_pos[0];
    delta_pos[1] = target_pos[1] - current_pos[1];
    return delta_pos;
}

void Plotter::home() {
    while (!limit_switch_left.is_pressed()) {
        motor_A.clockwise();
        motor_B.clockwise();
    }

    if (limit_switch_left.is_pressed()) {
        motor_A.stop();
        motor_B.stop();
        motor_A.anticlockwise_retreat();
        motor_B.anticlockwise_retreat();
        while (!limit_switch_left.is_pressed()) {
            motor_A.clockwise_approach();
            motor_B.clockwise_approach();
        }
        motor_A.stop();
        motor_B.stop();
    }

    while (!limit_switch_right.is_pressed()) {
        motor_A.anticlockwise();
        motor_B.anticlockwise();
    }

    if (limit_switch_right.is_pressed()) {
        motor_A.stop();
        motor_B.stop();
        motor_A.clockwise_retreat();
        motor_B.clockwise_retreat();
        while (!limit_switch_right.is_pressed()) {
            motor_A.anticlockwise_approach();
            motor_B.anticlockwise_approach();
        }
        motor_A.stop();
        motor_B.stop();
    }

    while (!limit_switch_top.is_pressed()) {
        motor_A.anticlockwise();
        motor_B.clockwise();
    }

    if (limit_switch_top.is_pressed()) {
        motor_A.stop();
        motor_B.stop();
        motor_A.clockwise_retreat();
        motor_B.anticlockwise_retreat();
        while (!limit_switch_top.is_pressed()) {
            motor_A.anticlockwise_approach();
            motor_B.clockwise_approach();
        }
        motor_A.stop();
        motor_B.stop();
    }

    while (!limit_switch_bottom.is_pressed()) {
        motor_A.clockwise();
        motor_B.anticlockwise();
    }

    if (limit_switch_bottom.is_pressed()) {
        motor_A.stop();
        motor_B.stop();
        motor_A.anticlockwise_retreat();
        motor_B.clockwise_retreat();
        while (!limit_switch_bottom.is_pressed()) {
            motor_A.clockwise_approach();
            motor_B.anticlockwise_approach();
        }
        motor_A.stop();
        motor_B.stop();
    }
}
