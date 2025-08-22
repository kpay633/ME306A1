// LATEST WORKING VERSION

// #ifndef TRAPEZOIDAL_PROFILE_HPP
// #define TRAPEZOIDAL_PROFILE_HPP
 
// class TrapezoidalProfile {
// public:
//     enum State {
//         ACCEL,
//         CRUISE,
//         DECEL,
//         FINISHED
//     };
 
// private:
//     // Positions and constraints
//     float x_start, y_start;
//     float x_final, y_final;
//     float max_vel;
//     float max_accel;
 
//     // Path properties
//     float total_dist;
//     float unit_dx, unit_dy;
 
//     // Motion profile parameters
//     float d_min;       // minimum distance to reach cruise velocity
//     float d_accel;     // distance covered during acceleration
//     float d_cruise;    // distance covered during cruise
//     float t_accel;     // time spent accelerating
//     float t_cruise;    // time spent cruising
//     float t_total;     // total move time
 
//     // Current state variables
//     float current_time;
//     float current_pos;
//     float current_vel;
//     State state;
 
// public:
//     TrapezoidalProfile(float x0, float y0, float x1, float y1, float vmax, float amax);
 
//     void update(float dt);
 
//     float getXSetpoint() const;
//     float getYSetpoint() const;
//     float getVXSetpoint() const;
//     float getVYSetpoint() const;
 
//     bool isFinished() const;
//     void reset();
 
// private:
//     float totalDistance(float x0, float y0, float x1, float y1) const;
// };
 
// #endif
 