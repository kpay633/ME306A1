\// LATEST WORKING VERSION

#ifndef PLOTTER_H
#define PLOTTER_H

class Motor;

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
        void MoveTime(int time, Target target, int speed);
        void move_to_target(float x, float y, float speed);
        float get_left_boundary();
        float get_right_boundary();
        float get_top_boundary();
        float get_bottom_boundary();
        void set_left_boundary(float boundary);
        void set_right_boundary(float boundary);
        void set_top_boundary(float boundary);
        void set_bottom_boundary(float boundary);
        void IncrementTime();
        Target GetAllowedSwitch1();
        Target GetAllowedSwitch2();

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
};

#endif // PLOTTER_H