#include <avr/io.h>
#include <Arduino.h>

#include "motor.hpp"

#define MOT1_PWM_PIN PB1
#define MOT2_PWM_PIN PB2

#define MOT1_ENCA_PIN PD0
#define MOT1_ENCB_PIN PD1
#define MOT2_ENCA_PIN PD2
#define MOT2_ENCB_PIN PD3

enum State {
    IDLE,
    HOMING,
    MOVING,
    FAULT
};


// void new_state(State);
// void doIdle(Plotter&);
// void doHoming(Plotter&);
// void doMoving(Plotter&);
// void doFault(Plotter&);
// State state = IDLE;

int main() {
    Plotter plotter;
    Serial.begin(9600);
    cli();
    Motor motor1(0, MotorID::M1, MOT1_PWM_PIN, MOT1_ENCA_PIN, MOT1_ENCB_PIN);  // voltage=0, timer=1, pwm_pin=PB1, enc_a=PD2, enc_b=PD3
    Motor motor2(0, MotorID::M2, MOT2_PWM_PIN, MOT2_ENCA_PIN, MOT2_ENCB_PIN);  // another motor on Timer2 with different encoder pins
    
    
    sei();
    
    while(1) {
      motor1.move_motor(MotorID::M1, 200, Direction::CCW);
      motor2.move_motor(MotorID::M2, 200, Direction::CCW);
      
      _delay_ms(1000);
      Serial.print("Encoder 1 - ");
      Serial.print(motor1.GetEncoderDist());
      Serial.print(" Encoder 2 - ");
      Serial.println(motor2.GetEncoderDist());

      motor1.stop_motor(MotorID::M1);
      motor2.stop_motor(MotorID::M2);
      

      _delay_ms(1000);
      Serial.print("Encoder 1 - ");
      Serial.print(motor1.GetEncoderDist());
      Serial.print(" Encoder 2 - ");
      Serial.println(motor2.GetEncoderDist());


      motor1.move_motor(MotorID::M1, 200, Direction::CW);
      motor2.move_motor(MotorID::M2, 200, Direction::CW);

      _delay_ms(1000);


      Serial.print("Encoder 1 - ");
      Serial.print(motor1.GetEncoderDist());
      Serial.print(" Encoder 2 - ");
      Serial.println(motor2.GetEncoderDist());


      
    //     key_in = getchar();
    //     g_code_extractor(key_in);
    //     switch(state) {
    //         case IDLE:
    //             if(key_in == *(G1)) new_state(HOMING);
    //             if(key_in == *(G28)) new_state(MOVING);
    //             doIdle(plotter);
    //             break;
    //         case HOMING:
    //             doHoming(plotter);
    //             break;
    //         case MOVING:
    //             doMoving(plotter);  // <--- motor control loop here
    //             break;
    //         case FAULT:
    //             doFault(plotter);
    //             break;
    //     }
    //     // delay ???
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100));
    // }
   // _delay_ms(1000);
  }
}

// void new_state(State s) {
//     state = s;
//     std::cout << "Transitioning to state: " << s << "\n";
// }
// void doIdle(Plotter& plotter) {
//     std::cout << "Idle ..." << "\n";
// }
// void doHoming(Plotter& plotter) {
//     plotter.home();
//     new_state(IDLE);
// }
// void doMoving(Plotter& plotter);
// void doFault(Plotter& plotter);