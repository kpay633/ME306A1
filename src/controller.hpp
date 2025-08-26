//LATEST WORKING VERSION

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP
 
enum class controlMode{
    P,
    P_I,
    PD,
    PID
};
 
class Controller {
private:
    //vars ofr the controller gains
    float Kp_x, Ki_x, Kd_x;
    float Kp_y, Ki_y, Kd_y;
 
    //vars for feed-forward control
    float Kv_x, Kv_y;
    float Ka_x, Ka_y;
 
    //vars for the states of the pid
    float integral_error_x;
    float integral_error_y;
    float prev_error_x;
    float prev_error_y;
 
    //timestep (this might need to come from the timer?)
    float dt;
 
    //outputs
    float motor_left_control_effort;
    float motor_right_control_effort;

    float output_min;
    float output_max;

    unsigned long last_time = 0;
 
public:
 
    Controller(float kp_x, float ki_x, float kd_x, float kp_y, float ki_y, float kd_y, float kv_x, float kv_y, float timestep);
    void calculateControlEffort(float current_error_x, float current_error_y, float v_desired_x, float v_desired_y, controlMode mode);
    void setGains(float kp_x, float ki_x, float kd_x, float kp_y, float ki_y, float kd_y);
    void reset();
    float getMotorLeftControlEffort() const;
    float getMotorRightControlEffort() const;
    void setSaturationLimits(float min_output, float max_output);
 
};
 
#endif // CONTROLLER_HPP