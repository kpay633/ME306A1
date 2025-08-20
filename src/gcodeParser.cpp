/*

  CHECK IF WE ARE ALLOWED TO USE THESE LIBRARIES??
  ???????????????????????????????????????????

*/

#include "gcodeParser.hpp"
#include <avr/io.h>

#include <math.h> 

GCodeParser::GCodeParser() : command_present(false) {}

    
    
    
    
bool GCodeParser::hasCommand() { // This is true if message has been received... hence can activate state transition
  return false;
}

int GCodeParser::getCommand() { // If hasCommand, will get command in main and then act on that. 
  return 0;
} 


GCodeCommand GCodeParser::parseLine(const char* user_input) {
  GCodeCommand cmd{CommandType::NONE, NAN, NAN};

  int i = 0;
  while (user_input[i] != '\0') {  // Extract useful info.
    char c = user_input[i];
    if (c == ' ' || c == '\t') { // Don't extract spaces.
      i++;
      continue;
    }

    if (c == 'G') {
      i++;
      float val = 0.0f;
      // int start = i;
      while ((user_input[i] >= '0' && user_input[i] <= '9')) {
        val = val * 10 + (user_input[i] - '0'); // (*10 shifts whats already there into the next decimal place)
        i++;
      }
      if (val == 1) cmd.type = CommandType::G1;
      else if (val == 28) {
        cmd.type = CommandType::G28;
        this->previous_command = cmd; // store before leaving
        return cmd;    
      }
      else {
        Serial.println("G-code command not recognised.");
        break;
      }
    }
    else if (c == 'X' || c == 'Y') {
      char axis = c;
      i++;
      float val = 0.0f;
      float sign = 1.0f;
      float decimalFactor = 0.0f;

      // Check for negative
      if (user_input[i] == '-') {
          sign = -1.0f;
          i++;
      }

      // Integer part
      while (user_input[i] >= '0' && user_input[i] <= '9') {
          val = val * 10 + (user_input[i] - '0');
          i++;
      }

      // Decimal part
      if (user_input[i] == '.') {
          i++;
          decimalFactor = 0.1f; // First digit after '.' is worth 1/10
          while (user_input[i] >= '0' && user_input[i] <= '9') {
              val += (user_input[i] - '0') * decimalFactor;
              decimalFactor *= 0.1f; // Next digit is worth 1/100, then 1/1000, etc.
              i++;
          }
      }

      val *= sign;
      if (axis == 'X') cmd.x = val;
      else if (axis == 'Y') cmd.y = val;
    }
    else {
        i++; // skip unknown characters
    }

    // Check case where user wanted to use previous x/y location so didn't input again
    if (cmd.type == CommandType::G1) {
        if (isnan(cmd.x)) {
            cmd.x = this->previous_command.x;
        } 
        if (isnan(cmd.y)) {
          cmd.y = this->previous_command.y;
        }
    }
    if (c == 'M') {
        i++;
        float val = 0.0f;
        while (user_input[i] >= '0' && user_input[i] <= '9') {
            val = val * 10 + (user_input[i] - '0');
            i++;
        }

        if (val == 999) {
            cmd.type = CommandType::M999;
            Serial.println("IMPLEMENT ERROR STATE RESET.");
            this->previous_command = cmd; // store before returning
            return cmd; // stop immediately
        } else {
            Serial.println("M-code command not recognised.");
        }
    }
  }
  this->previous_command = cmd;
  return cmd;
}




GCodeCommand parse_input() {
  Serial.println("Hello from Arduino main! Serial is working.");
  GCodeCommand command = CommandType::NONE; //HOW DO I INITIALISE THIS ALL AS NULL. 
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
  return command;
}

