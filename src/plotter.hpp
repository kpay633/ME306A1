#ifndef PLOTTER_HPP
#define PLOTTER_HPP

#include "motor.hpp"
#include "limit_switch.hpp"
#include "arduino.h"

// Define the homing steps as an enum for the non-blocking state machine
enum class HomingStep {
    NONE,                   // Homing not active
    MOVE_LEFT,
    RETREAT_RIGHT_1,
    MOVE_DOWN,
    RETREAT_UP_1,
    APPROACH_BOTTOM,
    APPROACH_LEFT, //
    MOVE_OUT_LITTLE_RIGHT,
    MOVE_OUT_LITTLE_UP,
    RESET_ORIGIN, //SET LEFT AND BOTTOM BOUNDARIES
    RETREAT_UP_2,
    MOVE_RIGHT,
    RETREAT_LEFT_1,
    APPROACH_RIGHT, //SET RIGHT BOUNDARY
    RETREAT_LEFT_2,
    MOVE_UP,
    RETREAT_DOWN_1,
    APPROACH_TOP, //SET TOP BOUNDARY
    RETREAT_DOWN_2,
    DONE                    // Homing process is complete
};


enum class Target{
    Left,
    Right,
    Up,
    Down,
    None,
};

class Plotter {
    public:
        Plotter(Motor* Motor_A, Motor* Motor_B);
        float *get_current_pos();
        void set_current_pos(float pos[2]);
        float *get_target_pos();
        void set_target_pos(float pos[2]);
        float *calc_pos_error(float current[2], float target[2]);
        void test();
        void MoveTo();
        void MoveDist(Target target, int distance);
        void home();
        void timer2_init();
        bool MoveTime(int move_time, Target target, int speed);
        void move_to_target(float x, float y, float speed);
        float get_left_boundary();
        float get_right_boundary();
        float get_top_boundary();
        float get_bottom_boundary();
        void set_left_boundary(float boundary);
        void set_right_boundary(float boundary);
        void set_top_boundary(float boundary);
        void set_bottom_boundary(float boundary);
        Target GetAllowedSwitch1();
        Target GetAllowedSwitch2();
        void start_homing();
        void homing_tick();
        bool is_homing_done();
        bool is_move_time_done();
        void IncrementTime();

    private:
        float current_pos[2];
        float target_pos[2];
        float delta_pos[2];
        float left_boundary, right_boundary, top_boundary, bottom_boundary;
        Motor* motor_A;
        Motor* motor_B;
        int time;
        Target allowed_switch = Target::None;
        Target allowed_switch2 = Target::None;
        HomingStep homing_step = HomingStep::NONE;
        bool is_moving = false;

};


#endif // PLOTTER_H
