#include <avr/io.h>
#include <util/delay.h> 

int main(void) {
    cli(); 

    DDRB |= (1 << DDB7);
    sei(); 

    while (1) {
        PORTB |= (1 << PB7);  
        _delay_ms(1000);      
        PORTB &= ~(1 << PB7); 
        _delay_ms(1000);      
    }
}
