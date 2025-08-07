#include "motor.hpp"
#include <avr/io.h>
#include "encoder.hpp"

Motor::Motor(int voltage, Timer timer, int pwm_pin, int enc_a_pin, int enc_b_pin)
  : voltage(voltage), timer(timer), pwm_pin(pwm_pin), enc_a_pin(enc_a_pin), enc_b_pin(enc_b_pin) {

  switch (timer) {
    case Timer::TIMER3A:
      // PE3 → OC3A → D5
      DDRE |= (1 << PE3);  // Set PE3 as output

      // Fast PWM, 8-bit: WGM3 = 0b0101
      TCCR3A = (1 << COM3A1) | (1 << WGM30);
      TCCR3B = (1 << WGM32) | (1 << CS31); // Prescaler = 8
      OCR3A = voltage;
      break;

    case Timer::TIMER4A:
      // PH3 → OC4A → D6
      DDRH |= (1 << PH3);  // Set PH3 as output

      TCCR4A = (1 << COM4A1) | (1 << WGM40);
      TCCR4B = (1 << WGM42) | (1 << CS41); // Prescaler = 8
      OCR4A = voltage;
      break;
  }

  myEncoder = new Encoder(motorNumber);
  
}

int Motor::get_voltage() const {
  return voltage;
}

void Motor::move_motor(int new_voltage) {
  if (new_voltage < 0) new_voltage = 0;
  if (new_voltage > 255) new_voltage = 255;

  voltage = new_voltage;

  switch (timer) {
    case Timer::TIMER3A:
      OCR3A = voltage;
      break;
    case Timer::TIMER4A:
      OCR4A = voltage;
      break;
  }
}

int Motor::GetEncoderDist() {
    return myEncoder->GetEncoderDist();
}