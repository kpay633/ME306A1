#include <avr/io.h>

int E1 = 5;
int M1 = 4;
int E2 = 6;
int M2 = 7;
void setup()
{
cli();

Serial.begin(115200);

pinMode(M1, OUTPUT);
pinMode(M2, OUTPUT);
pinMode(2, INPUT);
pinMode(3, INPUT);

EICRB |= (1 << ISC40) | (1 << ISC50);
EICRB &= ~((1 << ISC41) | (1 << ISC51));
EIMSK |= (1 << INT4) | (1 << INT5);

sei();
}

int current_state = 0x00;
int previous_state = 0x00;
int temp_state = 0x00;
uint16_t turns = 32767;

enum Direction {
  FORWARD = 1,
  BACKWARD = 0,
};

void loop()
{
int value = 200;

digitalWrite(M1,HIGH);
digitalWrite(M2, HIGH);
analogWrite(E1, value); //PWM Speed Control
analogWrite(E2, value); //PWM Speed Control

Serial.print(current_state,BIN);
Serial.print(" ");
Serial.print(turns);
Serial.println();

}

ISR(INT4_vect) {
  current_state ^= 0x01;
  if (current_state > previous_state){
    turns++;
  } else if ((current_state == 0b00) && (previous_state == 0b11)) {
    turns++;
  } else {
    turns--;
  }
  previous_state = current_state;
}

ISR(INT5_vect) {
  current_state ^= 0x02;
  if (current_state > previous_state){
    turns++;
  } else if ((current_state == 0b00) && (previous_state == 0b11)) {
    turns++;
  } else {
    turns--;
  }
  previous_state = current_state;
}
