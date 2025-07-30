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
    // PID logic for X and Y axes
    float P_output_x = this->Kp_x * delta_x;
    float P_output_y = this->Kp_y * delta_y;

    //this is a bit wront
    // float I_ouput_x = this->Ki_x * integral_error_x;
    // float I_ouput_y = this->Ki_y * integral_error_y;
    // float D_output_x = this->Kd_x * dt;
    // float D_output_y = this->Kd_y * dt;

    // float control_output_x = P_output_x+ I_ouput_x +D_output_x;
    // float control_output_y = P_output_y+ I_ouput_y +D_output_y;




    // Convert to motor L and R outputs
}

// Set new gain values
void Controller::setGains(float kp_x, float ki_x, float kd_x,
                          float kp_y, float ki_y, float kd_y) {
    // Assign new gain values
}

// Reset internal state
void Controller::reset() {
    // Zero out integrals and previous errors
}
