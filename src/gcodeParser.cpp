#include "gcodeParser.hpp"
#include <Arduino.h>
#include <math.h>


GCodeParser::GCodeParser() 
    : command_present(false), 
      previous_command{CommandType::NONE, NAN, NAN} {}

bool GCodeParser::hasCommand() { // This is true if message has been received... hence can activate state transition
  return false;
}

int GCodeParser::getCommand() { // If hasCommand, will get command in main and then act on that. 
  return 0;
} 


GCodeCommand GCodeParser::parseLine(const char* line) {
  GCodeCommand cmd{CommandType::NONE, NAN, NAN};

  int i = 0;
  while (line[i] != '\0') {  // Extract useful info.
    char c = line[i];
    if (c == ' ' || c == '\t') { // Don't extract spaces.
      i++;
      continue;
    }

    if (c == 'G') {
      i++;
      float val = 0.0f;
      // int start = i;
      while ((line[i] >= '0' && line[i] <= '9')) {
        val = val * 10 + (line[i] - '0'); // (*10 shifts whats already there into the next decimal place)
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
      if (line[i] == '-') {
          sign = -1.0f;
          i++;
      }

      // Integer part
      while (line[i] >= '0' && line[i] <= '9') {
          val = val * 10 + (line[i] - '0');
          i++;
      }

      // Decimal part
      if (line[i] == '.') {
          i++;
          decimalFactor = 0.1f; // First digit after '.' is worth 1/10
          while (line[i] >= '0' && line[i] <= '9') {
              val += (line[i] - '0') * decimalFactor;
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
        while (line[i] >= '0' && line[i] <= '9') {
            val = val * 10 + (line[i] - '0');
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














// Non-blocking function to check for and parse a single command line
GCodeCommand GCodeParser::check_user_input() {
  GCodeCommand command{CommandType::NONE, NAN, NAN};

  // Only process one character at a time to stay non-blocking
  if (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (this->idx > 0) {
        this->user_input[this->idx] = '\0'; // Null-terminate the string
        command = this->parseLine(this->user_input);
        
        Serial.print("Results: ");
        Serial.print((int)command.type);
        Serial.print(", ");
        Serial.print(command.x);
        Serial.print(", ");
        Serial.println(command.y);

        this->idx = 0; // Reset for the next command
      } else {
        // Ignore stray CR/LF
      }
    } else {
      if (this->idx < sizeof(this->user_input) - 1) {
        this->user_input[this->idx++] = c; // Add character to buffer
      } else {
        // Handle buffer overflow if needed
        this->idx = 0; // Reset or throw an error
      }
    }
  }
  
  return command;
}