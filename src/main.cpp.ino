  #include <avr/io.h>
  #include <Arduino.h>


  #include <avr/io.h>
  #include <avr/interrupt.h>

  // #include "motor.hpp"
  #include "gcodeParser.hpp"

  // volatile unsigned long sys_ticks = 0;  // should increment every 1ms


  enum class State {
      IDLE,
      MOVING,
      HOMING,
      FAULT
  };



  void new_state(State);

  // void doIdle(Plotter&);
  // void doHoming(Plotter&);
  // void doMoving(Plotter&);
  // void doFault(Plotter&);

  State global_state = State::IDLE;
  GCodeParser parser;
  GCodeCommand cmd;


  int main() {
    cli();
    // timer1_init();
    sei();
    // Plotter plotter; 


    init(); // needed when bypassing Arduino's default main
    Serial.begin(9600);
    while (!Serial) { } // wait for USB serial connection
    delay(100); 


    while (1) {
      cmd = parser.check_user_input();  // will return NONE if no new input

      switch(global_state) {
        case State::IDLE:
          if(cmd.type == CommandType::G1) {
            new_state(State::MOVING); 
          }
          else if(cmd.type == CommandType::G28) {
            new_state(State::HOMING);
          }
          else if(cmd.type == CommandType::M999) {
            // doFault(plotter);
            Serial.println("Switching state to FAULT.");

          }
          // doIdle(plotter);
          break;



        case State::MOVING:
          // doMoving(plotter);
          Serial.println("now its going to pass in x and y int moving motor");
          break;
          
        case State::HOMING:
          // doHoming(plotter);
          Serial.println("yay homing");

          break;

        case State::FAULT:
          // doFault(plotter);
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


  // void doIdle(Plotter& plotter) {

  // }

  // void doMoving(Plotter& plotter) {
  //   plotter.position(x, y);
  // }

  // void doHoming(Plotter& plotter) {
  //     plotter.home();
  //     new_state(IDLE);
  // }
  // void doFault(Plotter& plotter) {
  //   //print an error message and stop everything. 
  // }








  // ISR(TIMER1_COMPA_vect) {
  //     sys_ticks++;  // 1ms has passed
  // }

  // void timer1_init() {
  //     cli(); // disable interrupts while configuring

  //     TCCR1A = 0;  // normal mode
  //     TCCR1B = 0;

  //     // CTC mode (Clear Timer on Compare)
  //     TCCR1B |= (1 << WGM12);

  //     // Compare match every 1ms
  //     OCR1A = 249;  
  //     // formula: OCR1A = (F_CPU / (prescaler * 1000)) - 1
  //     // with F_CPU=16MHz, prescaler=64 → OCR1A=249

  //     // Enable interrupt on compare A
  //     TIMSK1 |= (1 << OCIE1A);

  //     // Start timer with prescaler = 64
  //     TCCR1B |= (1 << CS11) | (1 << CS10);

  //     sei(); // enable global interrupts
  // }
