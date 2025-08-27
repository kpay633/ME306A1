#include "gcodeParser.hpp"
#include <Arduino.h>
#include <math.h>


GCodeParser::GCodeParser() 
    : command_present(false), 
      previous_command{CommandType::NONE, NAN, NAN, NAN} {}

bool GCodeParser::hasCommand() { // This is true if message has been received... hence can activate state transition
  return false;
}

int GCodeParser::getCommand() { // If hasCommand, will get command in main and then act on that. 
  return 0;
} 

GCodeCommand GCodeParser::parseLine(const char* line) {
    GCodeCommand cmd;
    cmd.type = CommandType::NONE;
    cmd.x = NAN;
    cmd.y = NAN;
    cmd.f = NAN;

    int i = 0;
    while (line[i] != '\0') {
        char c = line[i];
        if (c == ' ' || c == '\t') { // Skip whitespace
            i++;
            continue;
        }

        if (c == 'G') {
            i++;
            int val = 0;
            while (line[i] >= '0' && line[i] <= '9') {
                val = val * 10 + (line[i] - '0');
                i++;
            }
            if (val == 1) {
                cmd.type = CommandType::G01;
            } else if (val == 28) {
                cmd.type = CommandType::G28;
            } else {
                Serial.println("G-code command not recognised.");
                break;
            }
        }
        else if (c == 'M') {
            i++;
            int val = 0;
            while (line[i] >= '0' && line[i] <= '9') {
                val = val * 10 + (line[i] - '0');
                i++;
            }
            if (val == 999) {
                cmd.type = CommandType::M999;
            } else {
                Serial.println("M-code command not recognised.");
            }
        }
        else if (c == 'X' || c == 'Y' || c == 'F') {
            char axis = c;
            i++;
            
            double val = 0.0;
            double sign = 1.0;
            
            // Check for negative
            if (line[i] == '-') {
                sign = -1.0;
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
                double decimalFactor = 0.1;
                while (line[i] >= '0' && line[i] <= '9') {
                    val += (line[i] - '0') * decimalFactor;
                    decimalFactor *= 0.1;
                    i++;
                }
            }
            
            val *= sign;
            if (axis == 'X') {
                cmd.x = val;
            } else if (axis == 'Y') {
                cmd.y = val;
            } else if (axis == 'F') {
                cmd.f = val;
            }
        } else {
            i++;
        }
    }

    // Apply fallback values after parsing the entire line.
    if (cmd.type == CommandType::G01) {
        if (isnan(cmd.x)) {
            cmd.x = previous_command.x;
        }
        if (isnan(cmd.y)) {
            cmd.y = previous_command.y;
        }
    }
    
    if (isnan(cmd.f)) {
        cmd.f = previous_command.f;
    }

    // Handle commands that should return immediately.
    if (cmd.type == CommandType::G28 || cmd.type == CommandType::M999) {
        this->previous_command = cmd;
        return cmd;
    }

    // Store the command before returning for use on the next line.
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
        Serial.print(command.y);
        Serial.print(", ");
        Serial.println(command.f);

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