#ifndef PLOTTER_H
#define PLOTTER_H
#include <stdint.h>

enum class Target {
  LEFT,
  RIGHT,
  UP,
  DOWN
};

class Plotter {
    public:
        Plotter();
        float *get_current_pos();
        void set_current_pos(float pos[2]);
        float *get_target_pos();
        void set_target_pos(float pos[2]);
        float *calc_pos_error(float current[2], float target[2]);
        void MoveMotorTime(int voltage, Target target, uint16_t time);
        void MoveMotors(int voltage, Target target);
        void StopMotors();
        void ResetEncoders();
        static void IncrementMotorTimer();
        void home();
<<<<<<< HEAD
        void test();
        void move_to_target(float x, float y, float speed);
=======
        void move_to_target(float x, float y, int speed);
>>>>>>> 2fb469a3443a680139b88f9acccccf10f1443cca
        float get_left_boundary();
        float get_right_boundary();
        float get_top_boundary();
        float get_bottom_boundary();
        void set_left_boundary(float boundary);
        void set_right_boundary(float boundary);
        void set_top_boundary(float boundary);
        void set_bottom_boundary(float boundary);
        static volatile uint16_t motor_timer;
        void MoveTo(int x_pos, int y_pos);

    private:
        float current_pos[2];
        float target_pos[2];
        float delta_pos[2];
        float left_boundary, right_boundary, top_boundary, bottom_boundary;
        
};

#endif // PLOTTER_H