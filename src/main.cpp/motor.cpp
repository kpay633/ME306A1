#include <avr/io.h>

int E1 = 5;
int M1 = 4;
int E2 = 6;
int M2 = 7;
void setup()
{
cli();

Serial.begin(9600);

pinMode(M1, OUTPUT);
pinMode(M2, OUTPUT);

DDRE &= ~((1 << PE5) | (1 << PE4));

EICRB |= (1 << ISC40);
EICRB &= ~(1 << ISC41);
EIMSK |= (1 << INT4);

sei();
}

uint16_t turns = 32767;

void loop()
{
int value = 100;

digitalWrite(M1,HIGH);
digitalWrite(M2, HIGH);
analogWrite(E1, value); //PWM Speed Control
analogWrite(E2, value); //PWM Speed Control

// Serial.print(current_state,BIN);
// Serial.print(" ");
Serial.print(turns);
Serial.println();

}

ISR(INT4_vect) {
  uint8_t pin_state = PINE;
  uint8_t a = (pin_state >> 4) & 0x01;
  uint8_t b = (pin_state >> 5) & 0x01;
  if (a == b) {
    turns++;
  } else {
    turns--;
  }
}
