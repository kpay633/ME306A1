#include "motor.hpp"
#include <avr/io.h>

Motor* Motor::motor1 = nullptr;
Motor* Motor::motor2 = nullptr;

Motor::Motor(int voltage, MotorID motorID, int pwm_pin, int enc_a_pin, int enc_b_pin)
  : voltage(voltage), motorID(motorID), pwm_pin(pwm_pin), enc_a_pin(enc_a_pin), enc_b_pin(enc_b_pin) {

  DDRD &= ~(1 << enc_a_pin);
  DDRD &= ~(1 << enc_b_pin);

  switch (motorID) {
    case MotorID::M1:
      // PE3 → OC3A → D5
      DDRE |= (1 << PE3);  // Set PE3 as output

      DDRG |= (1 << PG5); //DIRECTION CONTROL ENABLED


      // Fast PWM, 8-bit: WGM3 = 0b0101
      TCCR3A = (1 << COM3A1) | (1 << WGM30);
      TCCR3B = (1 << WGM32) | (1 << CS31); // Prescaler = 8
      OCR3A = voltage;

      DDRE &= ~(1 << enc_a_pin); //Set pins 20 and 21 as inputs for encoder signals
      DDRE &= ~(1 << enc_b_pin);

      EICRB |= (1 << ISC40); //Enable INT0 on rising edge
      EICRB &= ~(1 << ISC41);
      EIMSK |= (1 << INT4);

      pinA = enc_a_pin;
      pinB = enc_b_pin;

      motor1 = this;
      break;

    case MotorID::M2:
      // PH3 → OC4A → D6
      DDRH |= (1 << PH3);  // Set PH3 as output

      DDRH |= (1 << PH4); //DIR3ECTION CONTROL ENABLED


      TCCR4A = (1 << COM4A1) | (1 << WGM40);
      TCCR4B = (1 << WGM42) | (1 << CS41); // Prescaler = 8
      OCR4A = voltage;

      DDRE &= ~(1 << enc_a_pin); //Set pins 18 and 19 as inputs for encoder signals
      DDRE &= ~(1 << enc_b_pin);

      EICRB |= (1 << ISC50); //Enable INT2 on rising edge
      EICRB &= ~(1 << ISC51);
      EIMSK |= (1 << INT5);

      pinA = enc_a_pin;
      pinB = enc_b_pin;

      motor2 = this;
      break;
  }
}

// int Motor::get_voltage() const {
//   return voltage;
// }

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
  if (!disabled){
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
}

void Motor::DisableMotor(){
  disabled = true;
  stop_motor(MotorID::M1);
  stop_motor(MotorID::M2);
}

void Motor::EnableMotor(){
  disabled = false;
}


// void clockwise(int voltage, int ms);
// void anticlockwise(int voltage, int ms);

int Motor::GetEncoderDist() {
  // Serial.print("encoder value ");
  // Serial.println(encCount);
  return encCount; 
}

void Motor::ResetEncoder() {
  Serial.println("Resetting encoder");
  encCount = 0; // Reset the encoder count to zero
}

void Motor::ResetEncoder(int set_value){
  encCount = set_value;
}

void Motor::incrementEncoder() {
  uint8_t pin_state = PINE;
  uint8_t a = (pin_state & (1 << PE4)) ? 1 : 0;
  uint8_t b = (pin_state & (1 << PE0)) ? 1 : 0;

  if (a == b) {
    encCount++;
  } else {
    encCount--; 
  }
}

ISR(INT4_vect) {
  if(Motor::motor1 != nullptr) {
    Motor::motor1->incrementEncoder();
  }
}

ISR(INT5_vect) {
  if(Motor::motor2 != nullptr) {
    Motor::motor2->incrementEncoder();
  }
}