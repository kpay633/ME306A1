#ifndef GCODEPARSER_HPP
#define GCODEPARSER_HPP

#include <Arduino.h>

enum class CommandType { //enum class can only contain enumerators, not any other field
    NONE,
    G1,   // Move
    G28   // Home
};

struct GCodeCommand { //must be a struct as has specific types. 
    CommandType type;
    float x;
    float y;
};


class GCodeParser {
  private:
    bool command_present;
    GCodeCommand previous_command; // THIS IS FOR IF THEY DONT PUT IN A Y VAL, ASSUME IT WAS SAME AS LAST. 


  public:
    GCodeParser();
  
    bool hasCommand(); // This is true if message has been received... hence can activate state transition
    int getCommand(); // If hasCommand, will get command in main and then act on that. 
    GCodeCommand parseLine(const char* user_input);
};