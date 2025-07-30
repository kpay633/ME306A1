#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

class Controller {
private:
    
    //vars ofr the controller gains
    float Kp_x, Ki_x, Kd_x;
    float Kp_y, Ki_y, Kd_y;

    //vars for the states of the pid
    float integral_error_x;
    float integral_error_y;
    float prev_error_x;
    float prev_error_y;

    //timestep (this might need to come from the timer?)
    float dt;

public:

    Controller(float kp_x, float ki_x, float kd_x, float kp_y, float ki_y, float kd_y, float timestep);

    void calculateControlEffort(float delta_x, float delta_y, float& output_L, float& output_R);

    void setGains(float kp_x, float ki_x, float kd_x, float kp_y, float ki_y, float kd_y);
    void reset();

};

#endif // CONTROLLER_HPP