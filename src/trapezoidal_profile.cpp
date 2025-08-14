#include "trapezoidal_profile.hpp"
#include <cmath>

// Constructor calculates profile params once on init
TrapezoidalProfile::TrapezoidalProfile(float x0, float y0, float x1, float y1, float vmax, float amax)
    : x_start(x0), y_start(y0), x_final(x1), y_final(y1),
      max_vel(vmax), max_accel(amax),
      current_time(0.0f), current_pos(0.0f), current_vel(0.0f),
      state(ACCEL)
{
    total_dist = totalDistance(x_start, y_start, x_final, y_final);

    if (total_dist > 0.0f) {
        unit_dx = (x_final - x_start) / total_dist;
        unit_dy = (y_final - y_start) / total_dist;
    } else {
        unit_dx = 0.0f;
        unit_dy = 0.0f;
    }

    d_min = (max_vel * max_vel) / max_accel;

    if (total_dist <= 2 * d_min) {
        // Triangular profile: can't reach max_vel
        t_accel = std::sqrt(total_dist / max_accel);
        d_accel = 0.5f * max_accel * t_accel * t_accel;
        d_cruise = 0.0f;
        t_cruise = 0.0f;
    } else {
        // Trapezoidal profile
        t_accel = max_vel / max_accel;
        d_accel = 0.5f * max_accel * t_accel * t_accel;
        d_cruise = total_dist - 2 * d_accel;
        t_cruise = d_cruise / max_vel;
    }

    t_total = 2 * t_accel + t_cruise;
}

// Calculates Euclidean distance between two points
float TrapezoidalProfile::totalDistance(float x0, float y0, float x1, float y1) const {
    return std::sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

// Update FSM state and profile at each dt timestep
void TrapezoidalProfile::update(float dt) {
    if (state == FINISHED){
        return;
    } 

    //increment the time
    current_time += dt;

    switch (state) {
        case ACCEL:
            //at every check update velocity (v = a*(t-t0))
            current_vel += max_accel * dt;
            //make sure software knows current vel can't exceed hardware limits
            if (current_vel > max_vel){
                current_vel = max_vel;
            }

            //update position (d = v*t)
            current_pos += current_vel * dt;

            //if we should be past accel stage - either determined by position or time
            if (current_pos >= d_accel || current_time >= t_accel) {
                //go to cruise if trapezoidal, else go to decel mode
                if (d_cruise > 0.0f)
                    state = CRUISE;
                else
                    state = DECEL;
            }
            break;

        case CRUISE:
            //velocity s at max in cruise mode, and pos can be calc using (d = v*t)
            current_vel = max_vel;
            current_pos += current_vel * dt;

            //we are in trapezoidal mode anyway if in cruise, so go to decel if we are past cruse time or dist
            if (current_pos >= (d_accel + d_cruise) || current_time >= (t_accel + t_cruise)) {
                state = DECEL;
            }
            break;

        case DECEL:
            //same kinemaic forula
            current_vel -= max_accel * dt;
            //clipping velocity (i.e speed cant be negative)
            if (current_vel < 0.0f){
                current_vel = 0.0f;
            }
            //update pos
            current_pos += current_vel * dt;
            //after decel (by pos or time), we are done
            if (current_pos >= total_dist || current_vel <= 0.0f || current_time >= t_total) {
                current_pos = total_dist;
                current_vel = 0.0f;
                state = FINISHED;
            }
            break;

        case FINISHED:
            current_vel = 0.0f;
            current_pos = total_dist;
            break;
    }

    // Clamp current_pos not to exceed total_dist
    if (current_pos > total_dist) current_pos = total_dist;
}

// Return X setpoint along path
float TrapezoidalProfile::getXSetpoint() const {
    return x_start + unit_dx * current_pos;
}

// Return Y setpoint along path
float TrapezoidalProfile::getYSetpoint() const {
    return y_start + unit_dy * current_pos;
}

// Return X velocity setpoint
float TrapezoidalProfile::getVXSetpoint() const {
    return unit_dx * current_vel;
}

// Return Y velocity setpoint
float TrapezoidalProfile::getVYSetpoint() const {
    return unit_dy * current_vel;
}

bool TrapezoidalProfile::isFinished() const {
    return state == FINISHED;
}

// Reset profile to initial conditions
void TrapezoidalProfile::reset() {
    current_time = 0.0f;
    current_pos = 0.0f;
    current_vel = 0.0f;
    state = ACCEL;
}
