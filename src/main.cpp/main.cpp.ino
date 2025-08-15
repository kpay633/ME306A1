#include <avr/io.h>
#include <Arduino.h>

#include "motor.hpp"

enum class State {
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

State state = State::IDLE;

int main() {
    // cli();
    // Plotter plotter;
    // sei();

    while(1) {
      GCodeParser parser;

      parser.parseLine("G1 X10"); // parser.previous_command gets updated
      parser.parseLine("G1 Y20"); // parser uses parser.previous_command.x from before


      char user_input = '';
      while (Serial.available() > 0) {
          char c = Serial.read();
          if (c == '\n') break; // end of line
          input += c;            // append to string
      }





  //     key_in = getchar();
  //     g_code_extractor(key_in);
  //     switch(state) {
  //         case IDLE:
  //             if(key_in == *(G1)) new_state(State::HOMING);
  //             if(key_in == *(G28)) new_state(State::MOVING);
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
  //  _delay_ms(1000);

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
// void doMoving(Plotter& plotter) {
//   plotter.position(x, y);
// }
// void doFault(Plotter& plotter) {
//   //print an error message and stop everything. 
// }
