//this is where code for the contoller will go

#include "controller.hpp"

// Constructor
Controller::Controller(float kp_x, float ki_x, float kd_x, float kp_y, float ki_y, float kd_y, float timestep) {
    this->Kp_x = kp_x;
    this->Kp_y = kp_y;
    this->Ki_x = ki_x;
    this->Ki_y = ki_y;
    this->Kd_x = kd_x;
    this->Kd_y = kd_y;
    this->dt = timestep;

}

// Calculate control effort
void Controller::calculateControlEffort(float delta_x, float delta_y,
                                        float& output_L, float& output_R) {
    //PID logic for X and Y axes
    float P_output_x = this->Kp_x * delta_x;
    float P_output_y = this->Kp_y * delta_y;

    //integral calculation fro I term
    integral_error_x += delta_x * dt;
    integral_error_y += delta_y * dt;

    //I term
    float I_output_x = this->Ki_x * integral_error_x;
    float I_output_y = this->Ki_y * integral_error_y;

    //change in error calcualtion for D term
    float derivative_x = (delta_x - prev_error_x) / dt;
    float derivative_y = (delta_y - prev_error_y) / dt;
    
    //D term
    float D_output_x = this->Kd_x * derivative_x;
    float D_output_y = this->Kd_y * derivative_y;

    //Update last error to be current error
    prev_error_x = delta_x;
    prev_error_y = delta_y;

    //convert to motor L and R outputs or atleast figure out how
}

// Set new gain values (this could be sued for quick testing, 
//if we want to tune our gains through terminal without having to change code and recompile everytime)
void Controller::setGains(float kp_x, float ki_x, float kd_x,float kp_y, float ki_y, float kd_y) {
    this->Kp_x = kp_x;
    this->Kp_y = kp_y;
    this->Ki_x = ki_x;
    this->Ki_y = ki_y;
    this->Kd_x = kd_x;
    this->Kd_y = kd_y;    
}

// Reset internal state (the attributrs)
void Controller::reset() {
    // Zero out integrals and previous 
    this->integral_error_x = 0;
    this->integral_error_y = 0;
}
