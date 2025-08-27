// LATEST WORKING VERSION

#include <avr/io.h>
#include <Arduino.h>

#include "motor.hpp"
#include "plotter.hpp"
#include "limit_switch.hpp"
#include "gcodeParser.hpp"

// Define the pins for the motors and limit switches
#define MOT1_PWM_PIN PB1
#define MOT2_PWM_PIN PB2

#define MOT1_ENCA_PIN PD0
#define MOT1_ENCB_PIN PD1
#define MOT2_ENCA_PIN PD2
#define MOT2_ENCB_PIN PD3

volatile unsigned long sys_ticks = 0;   // should increment every 1ms

enum class State {
    IDLE,
    MOVING,
    HOMING,
    FAULT
};

void new_state(State);
void doMoving(float x, float y);
void doHoming();
void doFault();

State global_state = State::IDLE;
GCodeParser parser;
GCodeCommand cmd;

Motor* motor1;
Motor* motor2;
Plotter* plotter;


int main() {
    Serial.begin(9600);
    cli();
    
    motor1 = new Motor(MotorID::M1);
    motor2 = new Motor(MotorID::M2);
    plotter = new Plotter(motor1, motor2);
    // timer1_init();
    sei();

    while (1) {
        cmd = parser.check_user_input(); 

        switch(global_state) {
            case State::IDLE:
                if(cmd.type == CommandType::G01) {
                    new_state(State::MOVING);
                    doMoving(cmd.x, cmd.y, cmd.f);
                    break;
                }
                else if(cmd.type == CommandType::G28) {
                    new_state(State::HOMING);
                    doHoming();
                    break;

                }
                break;
            
            case State::MOVING:
                // In MOVING state, just check if the movement is complete
                // if (plotter->is_moving_done()) {
                //     new_state(State::IDLE);
                // }
                break;
                
            case State::HOMING:
                plotter->homing_tick();
                if (plotter->is_homing_done()) {
                  new_state(State::IDLE);
                  break;
                }
                break;

            case State::FAULT:
                // In FAULT state, motors are disabled and we wait for a reset
                // For now, we just remain in this state. A reset command (M999) could be used to clear it.
                if(cmd.type == CommandType::M999) {
                  // NEED OT RESET EVERYTHING HERE... LIKE ENCODERS MAYBE?

                  new_state(State::IDLE);
                  break;
                }
                break;
        }
    }
}


void new_state(State s) {
    global_state = s;
    Serial.println(static_cast<int>(s)); // Prints the integer value of the enum
}


void doMoving(float x, float y, float f) {
    Serial.print("Starting movement to X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.println(y);
    plotter->move_to_target(x, y, 100);
}


void doHoming() {
    Serial.println("Homing initiated.");
    plotter->start_homing();
}


void doFault() {
    Serial.println("!!! FAULT DETECTED. HALTING ALL OPERATIONS. !!!");
    motor1->stop_motor(MotorID::M1);
    motor2->stop_motor(MotorID::M2);
}







ISR(INT2_vect){
  if (sys_ticks < 50){
    return;
  }
  sys_ticks=0;
    if (global_state != State::HOMING){
      motor1->DisableMotor();
      motor2->DisableMotor();
      new_state(State::FAULT); 
    } else {
      if ((plotter->GetAllowedSwitch1() != Target::Up) && (plotter->GetAllowedSwitch2() != Target::Up)){
        motor1->DisableMotor();
        motor2->DisableMotor();
        new_state(State::IDLE);
      }
    }
}



ISR(INT3_vect){
  if (sys_ticks < 50){
    return;
  }
  sys_ticks=0;
    if (global_state != State::HOMING){
      motor1->DisableMotor();
      motor2->DisableMotor();
      new_state(State::IDLE);
    } else {
      if ((plotter->GetAllowedSwitch1() != Target::Down) && (plotter->GetAllowedSwitch2() != Target::Down)){
        motor1->DisableMotor();
        motor2->DisableMotor();
        new_state(State::IDLE);
      }
    }
}

ISR(INT4_vect){
  if (sys_ticks < 50){
    return;
  }
  sys_ticks=0;
    if (global_state != State::HOMING){
      motor1->DisableMotor();
      motor2->DisableMotor();
      new_state(State::IDLE);
    } else {
      if ((plotter->GetAllowedSwitch1() != Target::Right) && (plotter->GetAllowedSwitch2() != Target::Right)){
        motor1->DisableMotor();
        motor2->DisableMotor();
        new_state(State::IDLE);
      }
    }
}

ISR(INT5_vect){
  if (sys_ticks < 50){
    return;
  }
  sys_ticks=0;
    if (global_state != State::HOMING){
      motor1->DisableMotor();
      motor2->DisableMotor();
      new_state(State::IDLE);
    } else {
      if ((plotter->GetAllowedSwitch1() != Target::Left) && (plotter->GetAllowedSwitch2() != Target::Left)){
        motor1->DisableMotor();
        motor2->DisableMotor();
        new_state(State::IDLE);
      }
    }
}

ISR(TIMER2_OVF_vect) {
    plotter->IncrementTime();
}

ISR(TIMER1_COMPA_vect) {
      sys_ticks++;  // 1ms has passed
  }

void timer1_init() {
  cli(); // disable interrupts while configuring

  TCCR1A = 0;  // normal mode
  TCCR1B = 0;

  // CTC mode (Clear Timer on Compare)
  TCCR1B |= (1 << WGM12);

  // Compare match every 1ms
  OCR1A = 249;  
  // formula: OCR1A = (F_CPU / (prescaler * 1000)) - 1
  // with F_CPU=16MHz, prescaler=64 → OCR1A=249

  // Enable interrupt on compare A
  TIMSK1 |= (1 << OCIE1A);

  // Start timer with prescaler = 64
  TCCR1B |= (1 << CS11) | (1 << CS10);

  sei(); // enable global interrupts
}