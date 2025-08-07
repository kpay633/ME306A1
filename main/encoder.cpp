#include "encoder.hpp"

Encoder* Encoder::encoder1 = nullptr;
Encoder* Encoder::encoder2 = nullptr;

Encoder::Encoder(int motorNumber)
  : encCount(0) {
        if(motorNumber == 2){
            // Setup for motor two
            cli();

              DDRD &= ~(1 << PD3);
              DDRD &= ~(1 << PD2);

              EICRA |= (1 << ISC20);
              EICRA &= ~(1 << ISC21);
              EIMSK |= (1 << INT2);

              pinA = PD2;
              pinB = PD3;

              encoder2 = this;

              sei();
        } else if (motorNumber == 1){
            // Setup for motor one
            cli();

              DDRD &= ~(1 << PD1);
              DDRD &= ~(1 << PD0);

              EICRA |= (1 << ISC00);
              EICRA &= ~(1 << ISC01);
              EIMSK |= (1 << INT0);

              pinA = PD0;
              pinB = PD1;

              encoder1 = this;

              sei();
    }
  }

int Encoder::GetEncoderDist() {
  return int(double(encCount) * 13.5 * 3.14 / 172 / 24); // Convert counts to distance
}

void Encoder::ResetEncoder() {
  encCount = 0; // Reset the encoder count to zero
}

// void Encoder::SetPointer(Encoder* uniquePointer) {

// }

void Encoder::incrementEncoder() {
  uint8_t pin_state = PIND;
  uint8_t a = (pin_state >> pinA) & 0x01;
  uint8_t b = (pin_state >> pinB) & 0x01;

  if (a == b) {
    encCount++;
  } else {
    encCount--; 
  }
}

ISR(INT0_vect) {
  if(Encoder::encoder1 != nullptr) {
    Encoder::encoder1->incrementEncoder();
  }
}

ISR(INT2_vect) {
  if(Encoder::encoder2 != nullptr) {
    Encoder::encoder2->incrementEncoder();
  }
}