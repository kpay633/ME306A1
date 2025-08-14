#include <Arduino.h>
#include <avr/io.h>
#include "limit_switch.hpp"

// Create a limit switch on pin 22 (corresponds to PB4 on Arduino Mega)
Limit_Switch limit_switch(&DDRB, &PINB, &PORTB, 4);  // pin 22 = PB4

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for Serial to be ready
  Serial.println("Limit switch test starting...");
  
  // Debug: Print the register states after initialization
  Serial.print("DDRB: 0b");
  Serial.println(DDRB, BIN);
  Serial.print("PORTB: 0b");
  Serial.println(PORTB, BIN);
  Serial.print("PINB: 0b");
  Serial.println(PINB, BIN);
  Serial.println("Bit 4 should be: Input (DDRB bit 4 = 0), Pull-up enabled (PORTB bit 4 = 1)");
  Serial.println();
}

void loop() {
  // Read the raw pin value for debugging
  uint8_t pin_value = PINB;
  bool bit4_state = (pin_value & (1 << 4)) != 0;

  bool pressed = limit_switch.is_pressed();

  Serial.print("PINB raw: 0b");
  // Pad with leading zeros for 8-bit display
  for (int i = 7; i >= 0; i--) {
    Serial.print((pin_value >> i) & 1);
  }
  Serial.print(" (decimal: ");
  Serial.print(pin_value);
  Serial.print(") | Bit 4: ");
  Serial.print(bit4_state ? "HIGH" : "LOW");
  Serial.print(" | Switch: ");
  Serial.println(pressed ? "Pressed" : "Not pressed");

  delay(500); // Check every 500ms for easier reading
}
