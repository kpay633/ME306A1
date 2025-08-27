//LATEST WORKING VERSION

//this is where code for the controller will go
#include <avr/io.h>
#include "controller.hpp"
#include <Arduino.h>

 
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

    this->output_min = -105.0;  // -(255 - 150) = -105
    this->output_max = 105.0;   // (255 - 150) = 105
}
 
// Calculate control effort PID
void Controller::calculateControlEffort(float current_error_x, float current_error_y, float v_desired_x, float v_desired_y, controlMode mode) {

    unsigned long now = millis();
    if(last_time ==0){
        last_time = now;
    }
    this->dt = (now - last_time)/1000.0;
    last_time = now;

   
    //feed forward effort
    float ff_x = Kv_x * v_desired_x;
    float ff_y = Kv_y * v_desired_y;
   
    //PID logic for X and Y axes
    float P_output_x = this->Kp_x * current_error_x;
    float P_output_y = this->Kp_y * current_error_y;
 
    //integral calculation for I term (NOTE THIS WILL BE CONDITIONALLY UPDATED)
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
        case controlMode::P_I:
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

    //String unsaturated control ouptus for anti-windup logic
    float unsaturated_control_output_x = control_output_x;
    float unsaturated_control_output_y = control_output_y;

    control_output_x = constrain(control_output_x, output_min, output_max);
    control_output_y = constrain(control_output_y, output_min, output_max);

    bool update_integral_x = true;
    bool update_integral_y = true;

    if (mode == controlMode::P_I || mode == controlMode::PID) {
        //check for x axis saturation
        if (unsaturated_control_output_x > output_max && current_error_x > 0) {
            update_integral_x = false; // Don't wind up further in positive direction
        } else if (unsaturated_control_output_x < output_min && current_error_x < 0) {
            update_integral_x = false; // Don't wind up further in negative direction
        }
        
        //checking if y axis is saturated  
        if (unsaturated_control_output_y > output_max && current_error_y > 0) {
            update_integral_y = false;
        } else if (unsaturated_control_output_y < output_min && current_error_y < 0) {
            update_integral_y = false;
        }
        
        //update integrals only if allowed
        if (update_integral_x) {
            integral_error_x += current_error_x * dt;
        }
        if (update_integral_y) {
            integral_error_y += current_error_y * dt;
        }
    }
 
    //convert to motor L and R outputs or atleast figure out how
    this->motor_left_control_effort = control_output_x + control_output_y;
    this->motor_right_control_effort = control_output_x - control_output_y;


    // Serial.print(">Proportional Control Effort Contribution:");
    // Serial.println(P_output_x);
    // Serial.print(">Integral Control Effort Contribution");
    // Serial.println(I_output_x);
    // Serial.print(">Derivative Control Effort Contribution:");
    // Serial.println(D_output_x);
    
    // Serial.print(">Total Control Effort X:");
    // Serial.println(control_output_x);
    // Serial.print(">Total Control Effort Y:");
    // Serial.println(control_output_y);
    
    // Serial.print(">Control Effort to Left Motor:");
    // Serial.println(motor_left_control_effort);
    // Serial.print(">Control Effort to Right Motor:");
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

void Controller::setSaturationLimits(float min_output, float max_output) {
    this->output_min = min_output;
    this->output_max = max_output;
}
 
// Reset internal state (the attributrs)
void Controller::reset() {
    // Zero out integrals and previous
    this->integral_error_x = 0;
    this->integral_error_y = 0;
    this->prev_error_x = 0;
    this->prev_error_y = 0;
}