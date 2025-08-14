//this is where code for the controller will go
// #include <avr/io.h>
#include "controller.hpp"

// Constructor
Controller::Controller(float kp_x, float ki_x, float kd_x, float kp_y, float ki_y, float kd_y, float kv_x, float kv_y, float timestep) {
    this->Kp_x = kp_x;
    this->Kp_y = kp_y;
    this->Ki_x = ki_x;
    this->Ki_y = ki_y;
    this->Kd_x = kd_x;
    this->Kd_y = kd_y;
    this->Kv_x = kv_x;
    this->Kv_y = kv_y;
    this->dt = timestep;
}

// Calculate control effort PID
void Controller::calculateControlEffort(float current_error_x, float current_error_y, float v_desired_x, float v_desired_y, controlMode mode) {
    
    //feed forward effort
    float ff_x = Kv_x * v_desired_x;
    float ff_y = Kv_y * v_desired_y;
    
    //PID logic for X and Y axes
    float P_output_x = this->Kp_x * current_error_x;
    float P_output_y = this->Kp_y * current_error_y;

    //integral calculation for I term
    integral_error_x += current_error_x * dt;
    integral_error_y += current_error_y * dt;

    //I term
    float I_output_x = this->Ki_x * integral_error_x;
    float I_output_y = this->Ki_y * integral_error_y;

    //change in error calcualtion for D term
    float derivative_x = (current_error_x - prev_error_x) / dt;
    float derivative_y = (current_error_y - prev_error_y) / dt;
    
    //D term
    float D_output_x = this->Kd_x * derivative_x;
    float D_output_y = this->Kd_y * derivative_y;

    //Update last error to be current error
    prev_error_x = current_error_x;
    prev_error_y = current_error_y;
    
    //calculate control outputs, initialising them to feed forward
    float control_output_x = ff_x;
    float control_output_y = ff_y;

    switch (mode) {
        case controlMode::P:
            control_output_x += P_output_x;
            control_output_y += P_output_y;
            break;
        case controlMode::PI:
            control_output_x += P_output_x + I_output_x;
            control_output_y += P_output_y + I_output_y;
            break;
        case controlMode::PD:
            control_output_x += P_output_x + D_output_x;
            control_output_y += P_output_y + D_output_y;
            break;
        case controlMode::PID:
            control_output_x += P_output_x + I_output_x + D_output_x;
            control_output_y += P_output_y + I_output_y + D_output_y;
            break;
    }

    //TODO: convert to motor L and R outputs or atleast figure out how
    this->motor_left_control_effort = control_output_x + control_output_y;
    this->motor_right_control_effort = control_output_x - control_output_y;

    // //Print the control efforts to the console fr debugging purposes
    // Serial.print("Left motor control effort: ");
    // Serial.println(motor_left_control_effort);
    // Serial.print("Right motor control effort: ");
    // Serial.println(motor_right_control_effort);

}

float Controller::getMotorLeftControlEffort() const{
    return this->motor_left_control_effort;
}

float Controller::getMotorRightControlEffort() const{
    return this->motor_right_control_effort;
}

// Set new gain values (this could be used for quick testing, 
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
    this->prev_error_x = 0;
    this->prev_error_y = 0;
}


