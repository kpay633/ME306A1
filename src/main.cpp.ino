#include <avr/io.h>
#include <Arduino.h>


#include <avr/io.h>
#include <avr/interrupt.h>

// #include "motor.hpp"
#include "gcodeParser.hpp"

volatile unsigned long sys_ticks = 0;  // increments every 1ms

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
  cli();
  timer1_init();
  sei();

//-----G-CODE PARSER---------------------------------------------------------------------------------------------------------------
  init();                  // needed when bypassing Arduino's default main
  Serial.begin(9600);
  while (!Serial) { }      // wait for USB serial connection
  delay(100);              // small delay to stabilize

  Serial.println("Hello from Arduino main! Serial is working.");
  GCodeParser parser;
  GCodeCommand command; 
  char user_input[64];     // buffer for one command line
  size_t idx = 0;

  while (1) {
    // Check if serial has data
    while (Serial.available() > 0) {
      char c = Serial.read();

      if (c == '\n' || c == '\r') {   // end of command
        if (idx > 0) {              // only parse if buffer not empty
          user_input[idx] = '\0'; // terminate C string
          command = parser.parseLine(user_input);

          Serial.print("Results: "); 
          Serial.print((int)command.type);
          Serial.print(", ");
          Serial.print(command.x);
          Serial.print(", ");
          Serial.println(command.y);

          idx = 0;                // reset for next command
        } else {
          // ignore stray CR/LF when buffer is empty
        }
      } else {
        if (idx < sizeof(user_input) - 1) {
            user_input[idx++] = c;  // add char to buffer
        }
      }
    }
    delay(10);  // give USB stack time to process keyboard input
  }
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

//     }
// }

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
