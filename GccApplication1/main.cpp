/*
 * GccApplication1.cpp
 *
 * Created: 18.07.2018 20:38:17
 * Author : Misha
 */ 

#define F_CPU 1000000UL
#define es1 PIN1
#define es2 PIN0
#define butt PIN2
#define dir PIN3
#define step PIN4


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

bool ok = false;
bool changed = false;
volatile uint16_t counter = 0;

int main(void)
{
    /* Replace with your application code */
	
	
	DDRB |= (1<<dir) | (1<<step);
	DDRB &= ~((1<<es1) | (1<<es2) | (1<<butt));
	
	PORTB |= (1 << butt);
	PORTB &= ~(1<<dir);
	
	TCCR1 |= (1 << CTC1);  // clear timer on compare match
	TCCR1|=( (1<<CS13)|(1<<CS11)|(1<<CS10) ); // prescaler 64
	OCR1C = 5; // compare match value
	TIMSK |= (1 << OCIE1A); // enable compare match interrupt
	  
	GIMSK |= (1 << PCIE);   // pin change interrupt enable
	PCMSK |= (1<< PCINT0) | (1<<PCINT1) | (1 << PCINT2); // pin change interrupt enabled for PCINT4
	MCUCR |= (1 << ISC01) | (0 << ISC00); // Button trigers on raise
	sei();                  // enable interrupts
	
	
    while (1) 
    {		uint8_t c = 0;
			while((!(PINB &(1<<es2)) || !(PINB &(1<<es1))) && !changed && ok){
				
				c++;
				if (c >= 2){
					
					if (!(PINB &(1<<es2)) && PINB &(1<<dir)) PORTB ^= (1<<dir);
					
					if (!(PINB &(1<<es1)) && !(PINB &(1<<dir)) ){
						ok = false;
						PORTB |= (1<<dir);
						
						for (int i(0); i<250; i++){
							PORTB ^= (1<<step);
							_delay_ms(50);
						}
						PORTB &= ~(1<<dir);
						PORTB &= ~(1<<step);
						PORTB &= ~((1<<es1) | (1<<es2));
					}
					
					changed = true;
					c=0;
					counter = 0;
				}
			}
		}
}



ISR(PCINT0_vect){

// 	if (ok){
// 		PORTB ^= (1<<dir);	
// 		ok = false;	
// 	}
	if (!(PINB &(1<<butt)) && !changed && !ok) {
		PORTB |= (1<<dir);
		PORTB |= (1<<es1) | (1<<es2);
		ok = true;
		changed = true;
		counter = 0;
	}
	
	if (!(PINB &(1<<butt)) && !changed && ok) {
		ok = false;
		PORTB &= ~(1<<dir);
		PORTB &= ~(1<<step);
		PORTB &= ~((1<<es1) | (1<<es2));
		changed = true;
		counter = 0;
	}
		
	//ok = !ok;
}


ISR(TIMER1_COMPA_vect)
{	
	counter++;
	
	if(counter == 750){
		changed = false;
		counter = 0;
	}
	
	 if (ok) PORTB ^= (1<<step);
}