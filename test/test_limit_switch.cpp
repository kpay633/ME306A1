#include "limit_switch.hpp"

Limit_Switch left(2);
Limit_Switch right(3);
Limit_Switch bottom(4);
Limit_Switch top(5);

void setup() {
    Serial.begin(9600);
    sei(); // enable global interrupts
    Serial.println("Limit switch ISR test started!");
}

void loop() {
    static bool last_left=false, last_right=false, last_bottom=false, last_top=false;

    bool l = left.is_pressed();
    bool r = right.is_pressed();
    bool b = bottom.is_pressed();
    bool t = top.is_pressed();

    if (l != last_left) {
        Serial.print("Left switch "); Serial.println(l ? "PRESSED" : "RELEASED");
        last_left = l;
    }
    if (r != last_right) {
        Serial.print("Right switch "); Serial.println(r ? "PRESSED" : "RELEASED");
        last_right = r;
    }
    if (b != last_bottom) {
        Serial.print("Bottom switch "); Serial.println(b ? "PRESSED" : "RELEASED");
        last_bottom = b;
    }
    if (t != last_top) {
        Serial.print("Top switch "); Serial.println(t ? "PRESSED" : "RELEASED");
        last_top = t;
    }
}
