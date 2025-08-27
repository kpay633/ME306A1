#include "plotter.hpp"
#include "controller.hpp"

#define SWTOP PD2
#define SWBOTTOM PD3
#define SWRIGHT PE4
#define SWLEFT PE5

enum class InitCond{
    SWL,
    SWR,
    SWT,
    SWB,
    None
};

int nominal_speed = 200;
int approach_speed = 160;
int retreat_time = 500;
int retreat_speed = 200;
bool timer_done = true;

bool move_target_done = true;     

//MOVE TARGET VARIABLES
float motorA_move;
float motorB_move;
float max_abs;
float scale;
const float POSITION_TOLERANCE = 2;
float* current;
float delta_x;
float delta_y;
int iteration;
int32_t MAX_ITERATIONS = 30000;

  //parmaeters to ramp up velcocity
float initial_distance = 0.0;
float total_distance = 0.0;
bool ramp_up_phase = true;
const float RAMP_UP_FRACTION = 0.3; //this will make us ramp for first 30% of the way
const float MAX_VELOCITY_SCALE = 1.0; //the full cotnrol effort
const float MIN_VELOCITY_SCALE = 0.2; //starting control effort

//gettign the inital distance to the final target
float initial_delta_x;
float initial_delta_y;
float x_target;
float y_target;
float feed;

Limit_Switch limitSwitch;


Plotter::Plotter(Motor* Motor_A, Motor* Motor_B) : motor_A(Motor_A), motor_B(Motor_B) {
    current_pos[0] = 0.0;
    current_pos[1] = 0.0;
    target_pos[0] = 0.0;
    target_pos[1] = 0.0;
    delta_pos[0] = 0.0;
    delta_pos[1] = 0.0;
    left_boundary = 0.0;
    right_boundary = 0.0;
    top_boundary = 0.0;
    bottom_boundary = 0.0;
    time = 0;
    motor_A->ResetEncoder();
    motor_B->ResetEncoder();
    timer2_init();

}

void Plotter::timer2_init() {
  TCCR2A = 0;              // normal mode
  TCCR2B = _BV(CS22);      // prescaler = 64
  TIMSK2 = _BV(TOIE2);     // enable overflow interrupt
}

void Plotter::test(){
    motor_A->move_motor(MotorID::M1, 200, Direction::CW);
    motor_B->move_motor(MotorID::M2, 200, Direction::CW);

    //     while(1) {
    //   Serial.print("X ");
    //   Serial.print(get_current_pos()[0]);
    //   Serial.print(" Y ");
    //   Serial.println(get_current_pos()[1]);
//   }

}

void Plotter::MoveTo(){
   motor_A->ResetEncoder();
   motor_B->ResetEncoder();

   float error = 42.39 - motor_A->GetEncoderDist();
   int k_p = 0.5;
   int control_effort = int(error * k_p);
   Direction direction = Direction::CW;
   
   while(abs(error) > 1){
    error = 42.39 - motor_A->GetEncoderDist();
    control_effort = int(abs(error) * k_p + 140);
    if (control_effort > 255){
        control_effort = 255;
    }
    if (error < 0){
        direction = Direction::CW;
    } else {
        direction = Direction::CCW;
    }
    motor_A->move_motor(MotorID::M1, (control_effort), direction);
    Serial.print("ENC A ");
    Serial.print(motor_A->GetEncoderDist());
    Serial.print(" ERROR = ");
    Serial.println(error);

   }
   motor_A->stop_motor(MotorID::M1);
   motor_B->stop_motor(MotorID::M2);
}

bool Plotter::MoveTime(int move_time, Target target, int speed){
    if(timer_done == true) {
        timer_done = false;
        TCNT2 = 0;
        time = 0;
    }

    if(time >= move_time){
        timer_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return true;
    }

    switch (target) {
    case Target::Left:
        motor_A->move_motor(MotorID::M1, speed, Direction::CW);
        motor_B->move_motor(MotorID::M2, speed, Direction::CCW);
        return false;
    case Target::Right:
        motor_A->move_motor(MotorID::M1, speed, Direction::CCW);
        motor_B->move_motor(MotorID::M2, speed, Direction::CW);
        return false;
    case Target::Up:
        motor_A->move_motor(MotorID::M1, speed, Direction::CCW); 
        motor_B->move_motor(MotorID::M2, speed, Direction::CCW); 
        return false;
    case Target::Down:
        motor_A->move_motor(MotorID::M1, speed, Direction::CW); 
        motor_B->move_motor(MotorID::M2, speed, Direction::CW); 
        return false;
    case Target::None:
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return false;
    }
    return false;
}

void Plotter::move_to_target(float x_first, float y_first, float speed_first) {

    if ((limitSwitch.is_pressed(SWLEFT) || limitSwitch.is_pressed(SWRIGHT) || limitSwitch.is_pressed(SWTOP) || limitSwitch.is_pressed(SWBOTTOM))) {
        Serial.println("Error: Gantry on limit switch! Satying idle.");
        move_target_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return;
    }

    //DO ONCE AT START OF MOVing
    if(move_target_done){
        Serial.println("=== Simple move_to_target with Controller class ===");
        x_target = x_first;
        y_target = y_first;
        feed = speed_first / 500.0 * 110;
        Serial.print("FEED RATE = ");
        Serial.print(speed_first);
        x_target = current_pos[0] + x_target;
        y_target = current_pos[1] + y_target;
        iteration = 0;
        move_target_done = false;

         //parmaeters to ramp up velcocity
        initial_distance = 0.0;
        total_distance = 0.0;
        ramp_up_phase = true;
       
        //gettign the inital distance to the final target
        current = get_current_pos();
        initial_delta_x = x_target - current[0];
        initial_delta_y = y_target - current[1];
        initial_distance = sqrt(initial_delta_x * initial_delta_x + initial_delta_y * initial_delta_y);
        total_distance = initial_distance;  
    }

    //ARE ANY MOTORS DISABLED?
    if(motor_A->IsDisabled() || motor_B->IsDisabled()){
        Serial.println("Motors disabled");
        move_target_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return;
    }

  if ((x_target > get_right_boundary()) || (x_target < 0) || (y_target > get_top_boundary()) || (y_target < 0)){
        Serial.println("ERROR - OUTSIDE BOUNDS");
        move_target_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return;
    }

    if (iteration >= MAX_ITERATIONS) {
        Serial.println("Max iterations reached - stopping!");
        move_target_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return;
    }

    iteration++;

    //0.75 kp,0.6ki is best
    Controller ctrl(1, 0.1, 0.0, 1, 0.1, 0.0, 0.0, 0.0, 0.0);
    ctrl.setSaturationLimits(-105.0, 105.0);
        
    // Get current X/Y position
    current = get_current_pos();
    float delta_x = x_target - current[0];
    float delta_y = y_target - current[1];
    float current_distance = sqrt(delta_x * delta_x + delta_y * delta_y);

    //how far we travel
    float distance_traveled = total_distance - current_distance;
    float progress = 1.0;
    if(total_distance > 0){
        progress = (distance_traveled / total_distance);
    } 

    float velocity_scale = MAX_VELOCITY_SCALE;

    if (progress < RAMP_UP_FRACTION) {
        //ramp up phase means linear increase from MIN to MAX
        float ramp_progress = progress / RAMP_UP_FRACTION;
        velocity_scale = MIN_VELOCITY_SCALE + (MAX_VELOCITY_SCALE - MIN_VELOCITY_SCALE) * ramp_progress;
        ramp_up_phase = true;
    } else {
        //PI control takes over fully if we are over 30% of distance
        velocity_scale = MAX_VELOCITY_SCALE;
        ramp_up_phase = false;
    }

    // Teleplot: target vs current positions
    //CORRECTED Teleplot format
    // Serial.print(">Target X Position:");
    // Serial.println(x_target);
    // Serial.print(">Target Y Position:");
    // Serial.println(y_target);
    
    // Serial.print(">Current X Position:");
    // Serial.println(current[0]);
    // Serial.print(">Current Y Position:");
    // Serial.println(current[1]);
    
    // Serial.print(">Error in X position:");
    // Serial.println(delta_x);
    // Serial.print(">Error in Y position:");
    // Serial.println(delta_y);

    // Serial.print(">Distance Remaining:");
    // Serial.println(current_distance);
    // Serial.print(">Progress:");
    // Serial.println(progress);
    // Serial.print(">Velocity Scale:");
    // Serial.println(velocity_scale);
    // Serial.print(">Ramp Phase:");
    // Serial.println(ramp_up_phase ? 1 : 0);

    // Serial.print("Iter "); Serial.print(iteration);
    // Serial.print(" | Current: ("); Serial.print(current[0], 4);
    // Serial.print(", "); Serial.print(current[1], 4);
    // Serial.print(") | Counts: (MA "); Serial.print(motor_A->GetEncoderCounts());
    // Serial.print("MB "); Serial.print(motor_B->GetEncoderCounts());
    // Serial.print(") | Target: ("); Serial.print(x_target);
    // Serial.print(", "); Serial.print(y_target);
    // Serial.print(") | Error: ("); Serial.print(delta_x, 4);
    // Serial.print(", "); Serial.print(delta_y, 4);
    
    // Check if we're close enough
    if (abs(delta_x) < POSITION_TOLERANCE && abs(delta_y) < POSITION_TOLERANCE) {
        Serial.println("Position reached!");
        Serial.print("X error = ");
        Serial.print(delta_x);
        Serial.print(" Y error = ");
        Serial.print(delta_y);
        Serial.print("Iterations = ");
        Serial.println(iteration);
        move_target_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return;                                                                      //EXIT STATEMENT
    }

    // control_effort_X = delta_x * kp_x;
    // control_effort_Y = delta_y * kp_y;
    ctrl.calculateControlEffort(delta_x, delta_y, 0.0, 0.0, controlMode::P_I);
    
    motorA_move = ctrl.getMotorLeftControlEffort();
    motorB_move = ctrl.getMotorRightControlEffort();
    
    //appplying the velocity scaling

    // Move motors (proportional control would be better here)
    Direction dir_A = (motorA_move >= 0) ? Direction::CCW : Direction::CW;
    Direction dir_B = (motorB_move >= 0) ? Direction::CW : Direction::CCW;

    motorA_move = abs(motorA_move);
    motorB_move = abs(motorB_move);


    // Serial.print("Feed Rate = ");
    // Serial.println(feed);
    feed = (feed < 110) ? feed : 110;

    motorA_move *= velocity_scale;
    motorB_move *= velocity_scale;

    if((motorA_move > feed) || (motorB_move > feed)){
        max_abs = (motorA_move > motorB_move) ? motorA_move : motorB_move;
        scale = feed / max_abs;
        motorA_move = motorA_move * scale;
        motorB_move = motorB_move * scale;
    }

    // Serial.print("Feed = ");
    // Serial.print(feed);
    // Serial.print(" Motor A = ");
    // Serial.print(motorA_move);
    // Serial.print(" motor B = ");
    // Serial.println(motorB_move);

    // Serial.print("Motor moves - A: "); Serial.print(motorA_move, 4);
    // Serial.print(", B: "); Serial.println(motorB_move, 4);

    motor_A->move_motor(MotorID::M1, (motorA_move + 145), dir_A);
    motor_B->move_motor(MotorID::M2, (motorB_move + 145), dir_B);

}

void Plotter::move_to_origin() {
    Serial.println("=== Simple move_to_target with Controller class ===");
    //DO ONCE AT START OF MOVE
    if(move_target_done){
        x_target = 10.0;
        y_target = 10.0;

        iteration = 0;
        move_target_done = false;

         //parmaeters to ramp up velcocity
        initial_distance = 0.0;
        total_distance = 0.0;
        ramp_up_phase = true;
       
        //gettign the inital distance to the final target
        current = get_current_pos();
        initial_delta_x = x_target - current[0];
        initial_delta_y = y_target - current[1];
        initial_distance = sqrt(initial_delta_x * initial_delta_x + initial_delta_y * initial_delta_y);
        total_distance = initial_distance;  
    }

    //ARE ANY MOTORS DISABLED?
    if(motor_A->IsDisabled() || motor_B->IsDisabled()){
        Serial.println("Motors disabled");
        move_target_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return;
    }

  if ((x_target > get_right_boundary()) || (x_target < 0) || (y_target > get_top_boundary()) || (y_target < 0)){
        Serial.println("ERROR - OUTSIDE BOUNDS");
        move_target_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return;
    }

    if (iteration >= MAX_ITERATIONS) {
        Serial.println("Max iterations reached - stopping!");
        move_target_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return;
    }

    iteration++;

    //0.75 kp,0.6ki is best
    Controller ctrl(0.80, 0.6, 0.0, 0.75, 0.6, 0.0, 0.0, 0.0, 0.0);
    ctrl.setSaturationLimits(-105.0, 105.0);
        
    // Get current X/Y position
    current = get_current_pos();
    float delta_x = x_target - current[0];
    float delta_y = y_target - current[1];
    float current_distance = sqrt(delta_x * delta_x + delta_y * delta_y);

    //how far we travel
    float distance_traveled = total_distance - current_distance;
    float progress = 1.0;
    if(total_distance > 0){
        progress = (distance_traveled / total_distance);
    } 

    float velocity_scale = MAX_VELOCITY_SCALE;

    if (progress < RAMP_UP_FRACTION) {
        //ramp up phase means linear increase from MIN to MAX
        float ramp_progress = progress / RAMP_UP_FRACTION;
        velocity_scale = MIN_VELOCITY_SCALE + (MAX_VELOCITY_SCALE - MIN_VELOCITY_SCALE) * ramp_progress;
        ramp_up_phase = true;
    } else {
        //PI control takes over fully if we are over 30% of distance
        velocity_scale = MAX_VELOCITY_SCALE;
        ramp_up_phase = false;
    }

    
    Serial.print(">Error in X position:");
    Serial.println(delta_x);
    Serial.print(">Error in Y position:");
    Serial.println(delta_y);
    
    // Check if we're close enough
    if (sqrt(delta_x * delta_x + delta_y * delta_y) < POSITION_TOLERANCE) {
        Serial.println("Position reached!");
        move_target_done = true;
        motor_A->stop_motor(MotorID::M1);
        motor_B->stop_motor(MotorID::M2);
        return;                                                                      //EXIT STATEMENT
    }
    ctrl.calculateControlEffort(delta_x, delta_y, 0.0, 0.0, controlMode::P_I);
    
    motorA_move = ctrl.getMotorLeftControlEffort();
    motorB_move = ctrl.getMotorRightControlEffort();
    
    //appplying the velocity scaling
    motorA_move *= velocity_scale;
    motorB_move *= velocity_scale;

    // Move motors (proportional control would be better here)
    Direction dir_A = (motorA_move >= 0) ? Direction::CCW : Direction::CW;
    Direction dir_B = (motorB_move >= 0) ? Direction::CW : Direction::CCW;

    motorA_move = abs(motorA_move);
    motorB_move = abs(motorB_move);

    if((motorA_move > 110) || (motorB_move > 110)){
        max_abs = (motorA_move > motorB_move) ? motorA_move : motorB_move;
        scale = 110 / max_abs;
        motorA_move = motorA_move * scale;
        motorB_move = motorB_move * scale;
    }

    // Serial.print("Motor moves - A: "); Serial.print(motorA_move, 4);
    // Serial.print(", B: "); Serial.println(motorB_move, 4);

    motor_A->move_motor(MotorID::M1, (motorA_move + 145), dir_A);
    motor_B->move_motor(MotorID::M2, (motorB_move + 145), dir_B);

}

bool Plotter::IsMoveTargetDone(){
    return move_target_done;
}


float *Plotter::get_current_pos() {
    float a = motor_A->GetEncoderDist();
    float b = motor_B->GetEncoderDist();
    current_pos[0] = (a + b) / 2.0;
    current_pos[1] = (a - b) / 2.0;
    return current_pos;
}

void Plotter::set_current_pos(float pos[2]) {
    current_pos[0] = pos[0];
    current_pos[1] = pos[1];
}

float *Plotter::get_target_pos() {
    return target_pos;
}

void Plotter::set_target_pos(float pos[2]) {
    target_pos[0] = pos[0];
    target_pos[1] = pos[1];
}

float *Plotter::calc_pos_error(float current_pos[2], float target_pos[2]) {
    delta_pos[0] = target_pos[0] - current_pos[0];
    delta_pos[1] = target_pos[1] - current_pos[1];
    return delta_pos;
}

void Plotter::start_homing() {
    Serial.println("=== HOMING START ===");

    motor_A->EnableMotor();
    motor_B->EnableMotor();
    
    this->homing_step = HomingStep::INIT_ON_LEFT;

    if (limitSwitch.is_pressed(SWLEFT) && limitSwitch.is_pressed(SWTOP)) {
        allowed_switch = Target::Left;
        allowed_switch2 = Target::Up;
    } else if (limitSwitch.is_pressed(SWLEFT) && limitSwitch.is_pressed(SWBOTTOM)) {
        allowed_switch = Target::Left;
        allowed_switch2 = Target::Down;
    } else if (limitSwitch.is_pressed(SWRIGHT) && limitSwitch.is_pressed(SWTOP)) {
        allowed_switch = Target::Right;
        allowed_switch2 = Target::Up;
    } else if (limitSwitch.is_pressed(SWRIGHT) && limitSwitch.is_pressed(SWBOTTOM)) {
        allowed_switch = Target::Right;
        allowed_switch2 = Target::Down;
    } else if (limitSwitch.is_pressed(SWRIGHT)) {
        allowed_switch = Target::Right;
        allowed_switch2 = Target::None;
    } else if (limitSwitch.is_pressed(SWTOP)) {
        allowed_switch = Target::Up;
        allowed_switch2 = Target::None;
    } else if (limitSwitch.is_pressed(SWLEFT)) {
        allowed_switch = Target::Left;
        allowed_switch2 = Target::None;
    } else if (limitSwitch.is_pressed(SWBOTTOM)) {
        allowed_switch = Target::Down;
        allowed_switch2 = Target::None;
    }
}

void Plotter::homing_tick() {
    switch (this->homing_step) {
        
        case HomingStep::INIT_ON_LEFT:
            if (allowed_switch == Target::Left || allowed_switch2 == Target::Left){
                Serial.println("trying to move right");
                if(MoveTime(100, Target::Right, retreat_speed)) {
                    this->homing_step = HomingStep::INIT_ON_TOP;
                    Serial.println("moved right");                    
                    break;
                }
            } else {
                this->homing_step = HomingStep::INIT_ON_TOP;
            }
            break;

        case HomingStep::INIT_ON_TOP:
            if (allowed_switch == Target::Up || allowed_switch2 == Target::Up){
                Serial.println("trying to move down");
                if(MoveTime(100, Target::Down, retreat_speed)) {
                    this->homing_step = HomingStep::INIT_ON_RIGHT;
                    Serial.println("moved down");
                    break;
                }
            } else {
                this->homing_step = HomingStep::INIT_ON_RIGHT;
            }
            break;

        case HomingStep::INIT_ON_RIGHT:
            if (allowed_switch == Target::Right || allowed_switch2 == Target::Right){
                Serial.println("trying to move left");
                if(MoveTime(100, Target::Left, retreat_speed)) {
                    this->homing_step = HomingStep::INIT_ON_BOT;
                    Serial.println("moved left");
                    break;
                }
            } else {
                this->homing_step = HomingStep::INIT_ON_BOT;
            }
            break;


        case HomingStep::INIT_ON_BOT:
            if (allowed_switch == Target::Down || allowed_switch2 == Target::Down){
                Serial.println("trying to move up");
                if(MoveTime(100, Target::Up, retreat_speed)) {
                    this->homing_step = HomingStep::MOVE_LEFT;
                    Serial.println("moved up");
                    break;
                }
            } else {
                this->homing_step = HomingStep::MOVE_LEFT;
                break;
            }
            break;
        
        
        
        
        case HomingStep::MOVE_LEFT:
            Serial.println("Homing Step:MOVE LEFT");
            allowed_switch = Target::Left;
            allowed_switch2 = Target::None;
            if(limitSwitch.is_pressed(SWLEFT)) {
              this->homing_step = HomingStep::RETREAT_RIGHT_1;
              break;  
            }
            motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CW); 
            motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CCW);
            break;
            
        case HomingStep::RETREAT_RIGHT_1:
            Serial.println("Homing Step:RETREAT RIGHT 1");
            if (MoveTime(retreat_time, Target::Right, retreat_speed)) {
              this->homing_step = HomingStep::MOVE_DOWN;
              break;
            }
            break;

        case HomingStep::MOVE_DOWN:
            Serial.println("Homing Step:MOVE DOWN");
            allowed_switch = Target::Down;
            allowed_switch2 = Target::None; 
            if(limitSwitch.is_pressed(SWBOTTOM)) {
              this->homing_step = HomingStep::RETREAT_UP_1;
              break;  
            }
            motor_A->move_motor(MotorID::M1, nominal_speed-30, Direction::CW); 
            motor_B->move_motor(MotorID::M2, nominal_speed-30, Direction::CW);
            break;

        case HomingStep::RETREAT_UP_1:
            Serial.println("Homing Step:RETREAT UP");
            if (MoveTime(retreat_time, Target::Up, retreat_speed)) {
              this->homing_step = HomingStep::APPROACH_BOTTOM;
              break;
            }
            break;

        case HomingStep::APPROACH_BOTTOM:
            Serial.println("Homing Step:APPROACH_BOTTOM");
            allowed_switch = Target::Down;
            allowed_switch2 = Target::None; 
            if(limitSwitch.is_pressed(SWBOTTOM)) {
              this->homing_step = HomingStep::APPROACH_LEFT;
              break;  
            }
            motor_A->move_motor(MotorID::M1, approach_speed, Direction::CW); 
            motor_B->move_motor(MotorID::M2, approach_speed, Direction::CW);
            break;

        case HomingStep::APPROACH_LEFT:
            Serial.println("Homing Step:APPROACH_LEFT");
            allowed_switch = Target::Left;
            allowed_switch2 = Target::Down; 
            if(limitSwitch.is_pressed(SWLEFT)) {
                this->homing_step = HomingStep::MOVE_OUT_LITTLE_RIGHT;
                // allowed_switch = Target::None;
                // allowed_switch2 = Target::None; 
                // motor_A->stop_motor(MotorID::M1);
                // motor_B->stop_motor(MotorID::M2);
                break;  
            }
            motor_A->move_motor(MotorID::M1, approach_speed, Direction::CW); 
            motor_B->move_motor(MotorID::M2, approach_speed, Direction::CCW);
            break;




        case HomingStep::MOVE_OUT_LITTLE_RIGHT:
            if (MoveTime(retreat_time, Target::Right, retreat_speed)) {
              this->homing_step = HomingStep::MOVE_OUT_LITTLE_UP;
              break;
            }
            break;

            

        case HomingStep::MOVE_OUT_LITTLE_UP:
            if (MoveTime(retreat_time, Target::Up, retreat_speed)) {
              this->homing_step = HomingStep::RESET_ORIGIN;
              break;
            }
            break;

        case HomingStep::RESET_ORIGIN:
            motor_A->ResetEncoder();
            motor_B->ResetEncoder();
            set_left_boundary(0);
            set_bottom_boundary(0);


            set_right_boundary(170);
            set_top_boundary(120);
            this->homing_step = HomingStep::DONE;
            break;

        case HomingStep::RETREAT_UP_2:
            if (MoveTime(retreat_time, Target::Up, retreat_speed)) {
              this->homing_step = HomingStep::MOVE_RIGHT;
              break;
            }
            break;
  
        case HomingStep::MOVE_RIGHT:
            allowed_switch = Target::Right;
            allowed_switch2 = Target::None; 
            if(limitSwitch.is_pressed(SWRIGHT)) {
              this->homing_step = HomingStep::RETREAT_LEFT_1;
              break;  
            }
            Serial.println("Homing Step: MOVE_RIGHT");
            motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CCW); 
            motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CW);
            break; 

        case HomingStep::RETREAT_LEFT_1:
            if (MoveTime(retreat_time, Target::Left, retreat_speed)) {
              this->homing_step = HomingStep::APPROACH_RIGHT;
              break;
            }
            Serial.println("Homing Step: RETREAT_LEFT_1");
            break;

        case HomingStep::APPROACH_RIGHT:
            Serial.println("APPROACH RIGHT");
            allowed_switch = Target::Right;
            allowed_switch2 = Target::None; 
            if(limitSwitch.is_pressed(SWRIGHT)) {
              set_right_boundary(get_current_pos()[0]);
              this->homing_step = HomingStep::RETREAT_LEFT_2;
              break;  
            }
            motor_A->move_motor(MotorID::M1, approach_speed, Direction::CCW); 
            motor_B->move_motor(MotorID::M2, approach_speed, Direction::CW);
            break;

        case HomingStep::RETREAT_LEFT_2:
            Serial.println("RETREAT LEFT 2");
            if (MoveTime(retreat_time, Target::Left, retreat_speed)) {
              this->homing_step = HomingStep::MOVE_UP;
              break;
            }
            break;

        case HomingStep::MOVE_UP:
            Serial.println("MOVE UP");
            allowed_switch = Target::Up;
            allowed_switch2 = Target::None; 
            if(limitSwitch.is_pressed(SWTOP)) {
              this->homing_step = HomingStep::RETREAT_DOWN_1;
              break;  
            }
            motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CCW); 
            motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CCW);
            break;

        case HomingStep::RETREAT_DOWN_1:
            Serial.println("RETREAT DOWN 1");
            if (MoveTime(retreat_time, Target::Down, retreat_speed)) {
              this->homing_step = HomingStep::APPROACH_TOP;
              break;
            }
            break;

        case HomingStep::APPROACH_TOP:
            Serial.println("APPROACH TOP");
            allowed_switch = Target::Up;
            allowed_switch2 = Target::None; 
            if(limitSwitch.is_pressed(SWTOP)) {
              set_top_boundary(get_current_pos()[1]);
              this->homing_step = HomingStep::RETREAT_DOWN_2;
              break;  
            }
            motor_A->move_motor(MotorID::M1, approach_speed, Direction::CCW); 
            motor_B->move_motor(MotorID::M2, approach_speed, Direction::CCW);
            break;

        case HomingStep::RETREAT_DOWN_2:
            Serial.println("homing step: RETREAT DOWN 2");
            if (MoveTime(retreat_time, Target::Down, retreat_speed)) {
              this->homing_step = HomingStep::ORIGIN;
              allowed_switch = Target::None;
              allowed_switch2 = Target::None;
              Serial.println("=== HOMING COMPLETE ===");
              break;
            }
            break;

        case HomingStep::ORIGIN:
            // Serial.print("Left bound = ");
            // Serial.print(get_left_boundary());
            // Serial.print("Right bound = ");
            // Serial.print(get_right_boundary());
            // Serial.print("Top bound = ");
            // Serial.print(get_top_boundary());
            // Serial.print("Bottom bound = ");
            // Serial.println(get_bottom_boundary());
            move_to_origin();
            if(IsMoveTargetDone()) {
                this->homing_step = HomingStep::DONE;
                break;
            }  
            Serial.println("HOMING DONE _____________");

            break;

        case HomingStep::DONE:
            allowed_switch = Target::None;
            allowed_switch2 = Target::None; 
            motor_A->stop_motor(MotorID::M1);
            motor_B->stop_motor(MotorID::M2);
            break;

        case HomingStep::NONE:
            default:
            break;
    }
}

bool Plotter::is_homing_done() {
    return this->homing_step == HomingStep::DONE;
}



float Plotter::get_left_boundary() {
    return left_boundary;
}

float Plotter::get_right_boundary() {
    return right_boundary;
}

float Plotter::get_top_boundary() {
    return top_boundary;
}

float Plotter::get_bottom_boundary() {
    return bottom_boundary;
}

void Plotter::set_left_boundary(float boundary) {
    left_boundary = boundary;
}

void Plotter::set_right_boundary(float boundary) {
    right_boundary = boundary;
}

void Plotter::set_top_boundary(float boundary) {
    top_boundary = boundary;
}

void Plotter::set_bottom_boundary(float boundary) {
    bottom_boundary = boundary;
}



Target Plotter::GetAllowedSwitch1(){
    return allowed_switch;
}

Target Plotter::GetAllowedSwitch2(){
    return allowed_switch2;
}

void Plotter::IncrementTime() {
    time++;
}