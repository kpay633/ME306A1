#include "encoder.h"
Encoder* thisObject = nullptr;

Encoder::Encoder(int pinA, int pinB)
  : pinA(pinA), pinB(pinB), encCount(32768) {

cli();

DDRD &= ~(1 << pinA);
DDRD &= ~(1 << pinB);

EICRB |= (1 << ISC40);
EICRB &= ~(1 << ISC41);
EIMSK |= (1 << INT4);

thisObject = this;

sei();
}

int Encoder::GetEncoderDist() {
  return int(double(encCount) * 13.5 * 3.14 / 172 / 24); // Convert counts to distance
}

void Encoder::incrementEncoder() {
  uint8_t pin_state = PINE;
  uint8_t a = (pin_state >> 4) & 0x01;
  uint8_t b = (pin_state >> 5) & 0x01;

  if (a == b) {
    encCount++;
  } else {
    encCount--;
  }
}

ISR(INT4_vect) {
  thisObject->incrementEncoder();
}