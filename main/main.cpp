#include "encoder.h"
#include "motor.hpp"
#include <avr/io.h>
#include <Arduino.h>

Motor* motorOne = nullptr;

void setup() {
  Serial.begin(9600);
  motorOne = new Motor(150, 4, 2, 3);
}

void loop() {
  asm("nop");
  Serial.print("Encoder Distance: ");
  Serial.println(motorOne->GetEncoderDist());
  delay(100);
}
