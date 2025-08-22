// LATEST WORKING VERSION

#include <avr/io.h>
#include <Arduino.h>

#include "motor.hpp"
#include "plotter.h"
#include "limit_switch.hpp"
#include "gcodeParser.hpp"

#define MOT1_PWM_PIN PB1
#define MOT2_PWM_PIN PB2

// #define MOT1_ENCA_PIN PD0
// #define MOT1_ENCB_PIN PD1
// #define MOT2_ENCA_PIN PD2
// #define MOT2_ENCB_PIN PD3

#define SWTOP PD2
#define SWBOTTOM PD3
#define SWRIGHT PE4
#define SWLEFT PE5


// void new_state(State);
// void doIdle(Plotter&);
// void doHoming(Plotter&);
// void doMoving(Plotter&);
// void doFault(Plotter&);

volatile unsigned long sys_ticks = 0;  // should increment every 1ms


enum class State {
      IDLE,
      MOVING,
      HOMING,
      FAULT
  };

void new_state(State);
void doHoming();
void doMoving(float x, float y);
void doFault();


State global_state = State::IDLE;
GCodeParser parser;
GCodeCommand cmd;

Motor* motor1;
Motor* motor2;
Plotter* plotter;

int main() {
    Serial.begin(9600);

    // while (!Serial) { } // wait for USB serial connection
    // delay(100); 

    cli();

    motor1 = new Motor(MotorID::M1);  // voltage=0, timer=1, pwm_pin=PB1, enc_a=PD2, enc_b=PD3
    motor2 = new Motor(MotorID::M2);  // another motor on Timer2 with different encoder pins
    plotter = new Plotter(motor1, motor2);

    // timer1_init();


    sei();

    //init(); // needed when bypassing Arduino's default main




    while (1) {
      cmd = parser.check_user_input();  // will return NONE if no new input


      switch(global_state) {
        case State::IDLE:
          if(cmd.type == CommandType::G1) {
            new_state(State::MOVING); 
            doMoving(cmd.x, cmd.y);
          }
          else if(cmd.type == CommandType::G28) {
            new_state(State::HOMING);
          }
          else if(cmd.type == CommandType::M999) {
            Serial.println("Switching state to FAULT.");

          }
          // doIdle(plotter);
          break;



        case State::MOVING:
        Serial.print("X = ");
        Serial.print(cmd.x);
        Serial.print(" Y = ");
        Serial.println(cmd.y);
          doMoving(cmd.x, cmd.y);
          Serial.println("now its going to pass in x and y int moving motor");
          break;
          
        case State::HOMING:
          doHoming();
          Serial.println("yay homing");

          break;

        case State::FAULT:
          doFault();
          break;
      }
    }
    delay(100);
  }


void new_state(State s) {
  global_state = s;
  Serial.print("Switching state to ");
  Serial.println(static_cast<int>(s)); // Prints the integer value of the enum
}

void doIdle(Plotter& plotter) {

  }

void doMoving(float x, float y) {
    plotter->move_to_target(10, 110, 100);
    plotter->move_to_target(60, 110,100);
    plotter->move_to_target(60, 50, 100);
    plotter->move_to_target(10, 50,100);
    plotter->move_to_target(10, 110, 100);
    plotter->move_to_target(60, 50,100);
    plotter->move_to_target(60, 110,100);
    plotter->move_to_target(10, 50,100);
    

    // plotter->move_to_target(x, y, 100);
      new_state(State::IDLE);
    }

void doHoming() {
      plotter->home();
      // plotter->move_to_target(0, 0, 100);
      // plotter->move_to_target(plotter->get_right_boundary(), plotter->get_top_boundary(), 100);
 
      // plotter->move_to_target(100, 50, 100);


    // plotter->MoveTime(2000, Target::Left, 180);
    // plotter->MoveTime(2000, Target::Up, 180);
    // plotter->MoveTime(2000, Target::Right, 180);
    // plotter->MoveTime(2000, Target::Down, 180);

      new_state(State::IDLE);
  }

void doFault() {
    //print an error message and stop everything.
  }


ISR(INT2_vect){
  if (sys_ticks < 50){
    return;
  }
  sys_ticks=0;
    if (global_state != State::HOMING){
      motor1->DisableMotor();
      motor2->DisableMotor();
      new_state(State::IDLE); 
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