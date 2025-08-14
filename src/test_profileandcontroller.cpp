#include <iostream>
#include "trapezoidal_profile.hpp"
#include "controller.hpp"

int main() {
    // Simulation parameters
    float dt = 0.01f; // 10 ms control timestep
    float total_time = 5.0f; // simulate for 5 seconds max

    // Starting position
    float actual_x = 0.0f;
    float actual_y = 0.0f;

    // Target position
    float target_x = 10.0f;
    float target_y = 10.0f;

    // Trapezoidal profile parameters
    float vmax = 2.0f;    // max velocity units/s
    float amax = 1.0f;    // max acceleration units/s^2

    // Initialize trapezoidal profile
    TrapezoidalProfile profile(actual_x, actual_y, target_x, target_y, vmax, amax);

    // Initialize controller gains (tune these as needed)
    float kp_x = 1.0f, ki_x = 0.0f, kd_x = 0.1f;
    float kp_y = 1.0f, ki_y = 0.0f, kd_y = 0.1f;
    float kv_x = 1.0f, kv_y = 1.0f;

    Controller controller(kp_x, ki_x, kd_x, kp_y, ki_y, kd_y, kv_x, kv_y, dt);

    // Start trapezoidal motion
    profile.reset();

    float time = 0.0f;
    while (time < total_time && !profile.isFinished()) {
        // Update profile
        profile.update(dt);

        // Get setpoints from profile
        float setpoint_x = profile.getXSetpoint();
        float setpoint_y = profile.getYSetpoint();
        float desired_vx = profile.getVXSetpoint();
        float desired_vy = profile.getVYSetpoint();

        // Calculate error (for test, assume actual position lags control output slightly)
        float error_x = setpoint_x - actual_x;
        float error_y = setpoint_y - actual_y;

        // Compute control effort with PID mode
        controller.calculateControlEffort(error_x, error_y, desired_vx, desired_vy, controlMode::PID);

        // For this simple test, simulate actual position updating by applying control effort directly scaled
        // This is just for testing — replace with actual robot dynamics in real usage
        actual_x += controller.getMotorLeftControlEffort() * dt * 0.1f;  // 0.1 scaling factor
        actual_y += controller.getMotorRightControlEffort() * dt * 0.1f;

        // Print output
        std::cout << "Time: " << time << "s, "
                  << "Setpoint: (" << setpoint_x << ", " << setpoint_y << "), "
                  << "Error: (" << error_x << ", " << error_y << "), "
                  << "Control Effort (L,R): ("
                  << controller.getMotorLeftControlEffort() << ", "
                  << controller.getMotorRightControlEffort() << ")\n";

        time += dt;
    }

    std::cout << "Motion finished or time limit reached.\n";
    return 0;
}
