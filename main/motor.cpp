#include "motor.hpp"
#include <avr/io.h>

Motor* Motor::motor1 = nullptr;
Motor* Motor::motor2 = nullptr;

Motor::Motor(int voltage, Timer timer, int pwm_pin, int enc_a_pin, int enc_b_pin)
  : voltage(voltage), timer(timer), pwm_pin(pwm_pin), enc_a_pin(enc_a_pin), enc_b_pin(enc_b_pin) {

  DDRD &= ~(1 << enc_a_pin);
  DDRD &= ~(1 << enc_b_pin);

  switch (timer) {
    case Timer::M1_TIMER3A:
      // PE3 → OC3A → D5
      DDRE |= (1 << PE3);  // Set PE3 as output

      // Fast PWM, 8-bit: WGM3 = 0b0101
      TCCR3A = (1 << COM3A1) | (1 << WGM30);
      TCCR3B = (1 << WGM32) | (1 << CS31); // Prescaler = 8
      OCR3A = voltage;

      DDRD &= ~(1 << PD1); //Set pins 20 and 21 as inputs for encoder signals
      DDRD &= ~(1 << PD0);

      EICRA |= (1 << ISC00); //Enable INT0 on rising edge
      EICRA &= ~(1 << ISC01);
      EIMSK |= (1 << INT0);

      pinA = enc_a_pin;
      pinB = enc_b_pin;

      motor1 = this;
      break;

    case Timer::M2_TIMER4A:
      // PH3 → OC4A → D6
      DDRH |= (1 << PH3);  // Set PH3 as output

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

// int Motor::get_voltage() const {
//   return voltage;
// }


// int read_encoder() {

// }


// void stop_motor(void) {
//   switch (timer) {
//     case Timer::M1_TIMER3A:
//       OCR3A = 0;
//       break;
//     case Timer::M2_TIMER4A:
//       OCR4A = 0;
//       break;
//   }
// }




void Motor::move_motor(int new_voltage) {
  if (new_voltage < 0) new_voltage = 0;
  if (new_voltage > 255) new_voltage = 255;

  voltage = new_voltage;

  switch (timer) {
    case Timer::M1_TIMER3A:
      OCR3A = voltage;
      break;
    case Timer::M2_TIMER4A:
      OCR4A = voltage;
      break;
  }
}


// void fast_clockwise(int voltage, int ms);


// void fast_anticlockwise(int voltage, int ms);

int Motor::GetEncoderDist() {
  return int(double(encCount) * 13.5 * 3.14 / 172 / 24); // Convert counts to distance
}

void Motor::ResetEncoder() {
  encCount = 0; // Reset the encoder count to zero
}

void Motor::incrementEncoder() {
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
  if(Motor::motor1 != nullptr) {
    Motor::motor1->incrementEncoder();
  }
}

ISR(INT2_vect) {
  if(Motor::motor2 != nullptr) {
    Motor::motor2->incrementEncoder();
  }
}