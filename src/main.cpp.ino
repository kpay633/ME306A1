#include <avr/io.h>
#include <Arduino.h>


#include <avr/io.h>
#include <avr/interrupt.h>

// #include "motor.hpp"
#include "gcodeParser.hpp"

volatile unsigned long sys_ticks = 0;  // should increment every 1ms
State global_state;

enum class State {
    IDLE,
    HOMING,
    MOVING,
    FAULT
};

void new_state(State);
void doIdle(Plotter&);
void doHoming(Plotter&);
void doMoving(Plotter&);
void doFault(Plotter&);
GCodeCommand parse_input();



int main() {
  cli();
  timer1_init();
  sei();

  global_state = State::IDLE;
  GCodeCommand cmd;
  // Plotter plotter; 


  init();                  // needed when bypassing Arduino's default main
  Serial.begin(9600);
  while (!Serial) { }      // wait for USB serial connection
  delay(100); 


  while (1) {
    cmd = parse_input();  // will return NONE if no new input

    switch(global_state) {
      case State::IDLE:
        if(cmd.type == CommandType::G1) new_state(State::MOVING);
        else if(cmd.type == CommandType::G28) new_state(State::HOMING);
        else if(cmd.type == CommandType::M999) doFault(plotter);
        doIdle(plotter);
        break;

      case State::HOMING:
        doHoming(plotter);
        break;

      case State::MOVING:
        doMoving(plotter);
        break;

      case State::FAULT:
        doFault(plotter);
        break;
    }
  }


  delay(100);

}

void new_state(State s) {
    global_state = s;
    std::cout << "Transitioning to state: " << s << "\n";
}
void doIdle(Plotter& plotter) {
    std::cout << "Idle ..." << "\n";
}
void doHoming(Plotter& plotter) {
    plotter.home();
    new_state(IDLE);
}
void doMoving(Plotter& plotter) {
  plotter.position(x, y);
}
void doFault(Plotter& plotter) {
  //print an error message and stop everything. 
}




GCodeCommand parse_input() {
  Serial.println("Hello from Arduino main! Serial is working.");
  GCodeParser parser;
  GCodeCommand command; 
  char user_input[64];     // buffer for one command line
  size_t idx = 0;
  
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {   // end of command
      if (idx > 0) {              // only parse if buffer not empty
        user_input[idx] = '\0'; // terminate C string
        command = parser.parseLine(user_input);

        Serial.print("Results: "); Serial.print((int)command.type); Serial.print(", "); Serial.print(command.x); Serial.print(", "); Serial.println(command.y);
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
