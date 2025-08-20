// LATEST WORKING VERSION

#include "motor.hpp"
#include <avr/io.h>

Motor* Motor::motor1 = nullptr;
Motor* Motor::motor2 = nullptr;

#define MOT1_ENCA_PIN PJ0
#define MOT1_ENCB_PIN PJ1
#define MOT2_ENCA_PIN PK0
#define MOT2_ENCB_PIN PK1

#define MOT1_REG PINJ
#define MOT2_REG PINK


Motor::Motor(MotorID motorID)
  : motorID(motorID) {

  DDRD &= ~(1 << PD0);
  DDRD &= ~(1 << PD1);
  pcint_init();

  switch (motorID) {
    case MotorID::M1:
      // PE3 → OC3A → D5
      DDRE |= (1 << PE3);  // Set PE3 as output

      DDRG |= (1 << PG5); //DIRECTION CONTROL ENABLED


      // Fast PWM, 8-bit: WGM3 = 0b0101
      TCCR3A = (1 << COM3A1) | (1 << WGM30);
      TCCR3B = (1 << WGM32) | (1 << CS31); // Prescaler = 8
      OCR3A = voltage;

      DDRD &= ~(1 << PD1); //Set pins 20 and 21 as inputs for encoder signals
      DDRD &= ~(1 << PD0);

      EICRA |= (1 << ISC00); //Enable INT0 on rising edge
      EICRA &= ~(1 << ISC01);
      EIMSK |= (1 << INT0);

      pinA = PD0;
      pinB = PD1;

      motor1 = this;
      break;

    case MotorID::M2:
      // PH3 → OC4A → D6
      DDRH |= (1 << PH3);  // Set PH3 as output

      DDRH |= (1 << PH4); //DIR3ECTION CONTROL ENABLED


      TCCR4A = (1 << COM4A1) | (1 << WGM40);
      TCCR4B = (1 << WGM42) | (1 << CS41); // Prescaler = 8
      OCR4A = voltage;

      DDRD &= ~(1 << PD3); //Set pins 18 and 19 as inputs for encoder signals
      DDRD &= ~(1 << PD2);

      EICRA |= (1 << ISC20); //Enable INT2 on rising edge
      EICRA &= ~(1 << ISC21);
      EIMSK |= (1 << INT2);

      pinA = enc_a_pin;
      pinB = enc_b_pin;

      motor2 = this;
      break;
  }
}

void Motor::pcint_init(void) {
  // ------ MOTOR 1 ENCODER --------
  DDRJ &= ~((1 << MOT1_ENCA_PIN) | (1 << MOT1_ENCB_PIN)); 
  DDRK &= ~((1 << MOT2_ENCA_PIN) | (1 << MOT2_ENCB_PIN));

  // Enable interupt on PB4 (PCINT4) (D10)
  PCMSK1 |= (1 << PCINT9);
  PCMSK2 |= (1 << PCINT16);

  //Enable PCINT2 group
  PCICR |= (1 << PCIE1) | (1 << PCIE2);
}

void Motor::stop_motor(MotorID motorID) {
  switch (motorID) {
    case MotorID::M1:
      OCR3A = 0;
      break;
    case MotorID::M2:
      OCR4A = 0;
      break;
  }
}

void Motor::move_motor(MotorID motorID, int new_voltage, Direction direction) {
  if (new_voltage < 0) new_voltage = 0;
  if (new_voltage > 255) new_voltage = 255;

  voltage = new_voltage;

  switch (motorID) {
    case MotorID::M1:
      if (direction == Direction::CCW) {PORTG |= (1 << PG5);
      } 
      else if (direction == Direction::CW) {PORTG &= ~(1 << PG5);
      }

      OCR3A = voltage;
      break;

    case MotorID::M2:
      if (direction == Direction::CCW) {PORTH |= (1 << PH4);
      } 
      else if (direction == Direction::CW) {PORTH &= ~(1 << PH4);
      }

      OCR4A = voltage;
      break;
  }
}

int Motor::GetEncoderDist() {
  return int(double(encCount) * 13.5 * 3.14 / 172 / 24); // Convert counts to distance
}

void Motor::ResetEncoder() {
  encCount = 0; // Reset the encoder count to zero
}

void Motor::incrementEncoder1() {
  uint8_t pin_state = MOT1_REG;
  uint8_t a = (pin_state >> MOT1_ENCA_PIN) & 0x01;
  uint8_t b = (pin_state >> MOT1_ENCB_PIN) & 0x01;

  if (a == b) {
    encCount++;
  } else {
    encCount--; 
  }
}

void Motor::incrementEncoder2() {
  uint8_t pin_state = MOT2_REG;
  uint8_t a = (pin_state >> MOT2_ENCA_PIN) & 0x01;
  uint8_t b = (pin_state >> MOT2_ENCB_PIN) & 0x01;

  if (a == b) {
    encCount++;
  } else {
    encCount--; 
  }
}

ISR(PCINT1_vect) {
  if(Motor::motor1 != nullptr) {
    Motor::motor1->incrementEncoder1();
  }
}

ISR(PCINT2_vect) {
  if(Motor::motor2 != nullptr) {
    Motor::motor2->incrementEncoder2();
  }
}