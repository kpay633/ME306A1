#ifndef GCODEPARSER_HPP
#define GCODEPARSER_HPP

#include <Arduino.h>
#include <math.h> // for NAN

enum class CommandType { 
    NONE,
    G1,   // moving. position
    G28,  // home
    M999
};

struct GCodeCommand { 
    CommandType type;
    float x;
    float y;
};

class GCodeParser {
  private:
    bool command_present;
    GCodeCommand previous_command; // stores last X/Y
    GCodeCommand parseLine(const char* line);
    char user_input[128]; // Increased buffer size for safety
    int idx = 0;



  public:
    GCodeParser();
    bool hasCommand();
    int getCommand();
    GCodeCommand check_user_input(); 
};

#endif // GCODEPARSER_HPP
