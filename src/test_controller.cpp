#include <iostream>
#include <cassert>
#include "controller.hpp"

int main() {
    //float kp_x, float ki_x, float kd_x, float kp_y, float ki_y, float kd_y, float kv_x, float kv_y, float timestep
    Controller ctrl(1.0, 0.5, 0.1, 1.0, 0.5, 0.1, 1.0, 1.0, 0.1);

    //PID control with known values
    float error_x = 2.0f;
    float error_y = -1.0f;
    float v_des_x = 0.5f;
    float v_des_y = -0.5f;

    ctrl.calculateControlEffort(error_x, error_y, v_des_x, v_des_y, controlMode::PID);

    float leftEffort = ctrl.getMotorLeftControlEffort();
    float rightEffort = ctrl.getMotorRightControlEffort();

    std::cout << "Left Control Effort: " << leftEffort << std::endl;
    std::cout << "Right Control Effort: " << rightEffort << std::endl;

    // Check basic validity that a case with errors has cpntrol inputs applied
    assert(leftEffort != 0.0f); 
    assert(rightEffort != 0.0f);

    // Test reset
    ctrl.reset();
    ctrl.calculateControlEffort(0, 0, 0, 0, controlMode::PID);
    assert(ctrl.getMotorLeftControlEffort() == 0);
    assert(ctrl.getMotorRightControlEffort() == 0);

    // Test setGains
    ctrl.setGains(2.0, 0.0, 0.0, 2.0, 0.0, 0.0);
    ctrl.calculateControlEffort(1.0, 1.0, 0, 0, controlMode::P);
    float expectedLeft = 2.0 * 1.0 + 2.0 * 1.0;
    float expectedRight = 2.0 * 1.0 - 2.0 * 1.0;
    assert(ctrl.getMotorLeftControlEffort() == expectedLeft);
    assert(ctrl.getMotorRightControlEffort() == expectedRight);

    std::cout << "All tests passed!" << std::endl;
    return 0;

}
