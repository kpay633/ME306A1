#include "plotter.hpp"

#define SWTOP PD2
#define SWBOTTOM PD3
#define SWRIGHT PE4
#define SWLEFT PE5

int nominal_speed = 200;
int approach_speed = 160;
int retreat_time = 100;
int retreat_speed = 200;
bool timer_done = true;

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

void Plotter::move_to_target(float x_target, float y_target, float speed) {
    Serial.println("=== Simple move_to_target ===");
    Serial.print("Motors disabled = ");
    Serial.println(motor_A->IsDisabled());

    if ((x_target > get_right_boundary()) || (x_target < 0) || (y_target > get_top_boundary()) || (y_target < 0)){
        Serial.println("ERROR - OUTSIDE BOUNDS");
        return;
    }

    int kp_x = 4;
    int kp_y = 4;
    float control_effort_X;
    float control_effort_Y;
    float motorA_move;
    float motorB_move;
    float max_abs;
    float scale;

    const float POSITION_TOLERANCE = 1;
    int iteration = 0;
    const int MAX_ITERATIONS = 100; // Prevent infinite loop

    while (iteration < MAX_ITERATIONS) {
        iteration++;
        
        // Get current X/Y position
        float* current = get_current_pos();
        float delta_x = x_target - current[0];
        float delta_y = y_target - current[1];

        Serial.print("Iter "); Serial.print(iteration);
        Serial.print(" | Current: ("); Serial.print(current[0], 4);
        Serial.print(", "); Serial.print(current[1], 4);
        Serial.print(") | Counts: (MA "); Serial.print(motor_A->GetEncoderCounts());
        Serial.print("MB "); Serial.print(motor_B->GetEncoderCounts());
        Serial.print(") | Target: ("); Serial.print(x_target);
        Serial.print(", "); Serial.print(y_target);
        Serial.print(") | Error: ("); Serial.print(delta_x, 4);
        Serial.print(", "); Serial.print(delta_y, 4);
        

        // Check if we're close enough
        if (abs(delta_x) < POSITION_TOLERANCE && abs(delta_y) < POSITION_TOLERANCE) {
            Serial.println("Position reached!");
            break;
        }

        control_effort_X = delta_x * kp_x;
        control_effort_Y = delta_y * kp_y;

        motorA_move = control_effort_X + control_effort_Y;
        motorB_move = control_effort_X - control_effort_Y;
        // Convert X/Y error to motor movements


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

        // if (motorA_move > 220){
        //     motorA_move = 220;
        // }

        // if (motorB_move > 220){
        //     motorB_move = 220;
        // }

        Serial.print("Motor moves - A: "); Serial.print(motorA_move, 4);
        Serial.print(", B: "); Serial.println(motorB_move, 4);

        motor_A->move_motor(MotorID::M1, (motorA_move + 140), dir_A);
        motor_B->move_motor(MotorID::M2, (motorB_move + 140), dir_B);


        
        // // Stop motors after each iteration to see if position changed
        // motor_A->stop_motor(MotorID::M1);
        // motor_B->stop_motor(MotorID::M2);
        }
    
    if (iteration >= MAX_ITERATIONS) {
        Serial.println("Max iterations reached - stopping!");
    }

    // Stop motors
    motor_A->stop_motor(MotorID::M1);
    motor_B->stop_motor(MotorID::M2);
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

    if (limitSwitch.is_pressed(SWLEFT)){
        allowed_switch = Target::Left;
        Serial.println("trying to move Right");
        MoveTime(retreat_time, Target::Right, retreat_speed);
        Serial.println("moved Right");
    }
    if (limitSwitch.is_pressed(SWRIGHT)){
        allowed_switch = Target::Right;
        Serial.println("trying to move Left");
        MoveTime(retreat_time, Target::Left, retreat_speed);
        Serial.println("moved left");
    }
    if (limitSwitch.is_pressed(SWTOP)){
        allowed_switch = Target::Up;
        Serial.println("trying to move Down");
        MoveTime(retreat_time, Target::Down, retreat_speed);
        Serial.println("Moved down");
    }
    if (limitSwitch.is_pressed(SWBOTTOM)){
        allowed_switch = Target::Down;
        Serial.println("trying to move Up");
        MoveTime(retreat_time, Target::Up, retreat_speed);
        Serial.println("Moved up");
    }
    
    this->homing_step = HomingStep::MOVE_LEFT;
}

void Plotter::homing_tick() {
    switch (this->homing_step) {
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
            motor_A->move_motor(MotorID::M1, nominal_speed, Direction::CW); 
            motor_B->move_motor(MotorID::M2, nominal_speed, Direction::CW);
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
            this->homing_step = HomingStep::RETREAT_UP_2;
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
            break;

        case HomingStep::APPROACH_RIGHT:
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
            if (MoveTime(retreat_time, Target::Left, retreat_speed)) {
              this->homing_step = HomingStep::MOVE_UP;
              break;
            }
            break;

        case HomingStep::MOVE_UP:
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
            if (MoveTime(retreat_time, Target::Down, retreat_speed)) {
              this->homing_step = HomingStep::ORIGIN;
              allowed_switch = Target::None;
              allowed_switch2 = Target::None;
              break;
            }
            break;

        case HomingStep::ORIGIN:
            move_to_target(0, 0, 500);
            if(IsMoveToTargetDone()){
              this->homing_step = HomingStep::DONE;
              break;
            }
            Serial.println("=== HOMING COMPLETE ===");

        case HomingStep::DONE:
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