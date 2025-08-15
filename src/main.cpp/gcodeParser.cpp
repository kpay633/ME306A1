/*

  CHECK IF WE ARE ALLOWED TO USE THESE LIBRARIES??
  ???????????????????????????????????????????

*/

#include "gcodeParse.hpp"
#include <avr/io.h>

#include <sstream>
#include <string>
#include <cmath> // for NAN

GCodeParser::GCodeParser() : command_present(false) {}

    
    
    
    
bool hasCommand() { // This is true if message has been received... hence can activate state transition

}

int getCommand() { // If hasCommand, will get command in main and then act on that. 

} 


GCodeCommand parseLine(const char* user_input) {
  GCodeCommand cmd{CommandType::None, NAN, NAN};

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
        int start = i;
        while ((user_input[i] >= '0' && user_input[i] <= '9')) {
            val = val * 10 + (user_input[i] - '0'); // (*10 shifts whats already there into the next decimal place)
            i++;
        }
        if (val == 1) cmd.type = CommandType::G1;
        else if (val == 28) cmd.type = CommandType::G28;
        else Serial.println("G-code command not recognised.");
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
        } else if (isnan(cmd.y)) {
            cmd.y = this->previous_command.y;
        }
    }

    // At the very end, store current command as previous
    this->previous_command = cmd;
  }
  return cmd;
}