#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>  // For rand()

#define CHASER_DELAY_TIME 500
#define TWINKLE_DELAY_TIME 100

// Function prototypes
void init_leds(void);
void led_on(uint8_t led_number);
void led_off(uint8_t led_number);
void all_leds_off(void);
void single_led_chaser_cw(uint8_t number_of_cycles);
void single_led_chaser_ccw(uint8_t number_of_cycles);
void double_led_chaser(uint8_t number_of_cycles);
void twinkling_stars(uint8_t cycles, uint16_t base_delay_ms);
void delay_millis(uint16_t ms);


// Function to blink the LED connected to PA5 five times
void blink_five_times(void) {
	for (uint8_t i = 0; i < 5; i++) {
		PORTA.OUTSET = (1 << 5);  
		_delay_ms(500);           
		PORTA.OUTCLR = (1 << 5);  
		_delay_ms(500);           
	}
}

// Interrupt Service Routine for PORTA
ISR(PORTA_PORT_vect) {
	PORTA.INTFLAGS = (1 << 4);  // Clear the interrupt flag for PA4
}

int main(void) {
	
    // Set LEDs as output
	init_leds();
    
	// Switch on PA4. Set as input, pull up enable, interrupt
	PORTA.DIRCLR = (1 << 4);
	PORTA.PIN4CTRL = PORT_PULLUPEN_bm | PORT_ISC_BOTHEDGES_gc;  // Enable pull-up and both-edges interrupt
	
	// Set the sleep mode to Power-Down for low power consumption
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	// Perform the initial blinking sequence
	blink_five_times();
	
	// Main loop: Sleep, wake on interrupt, check pin state, blink if pressed, repeat
	while (1) {
		sleep_enable();      // Enable sleep mode
		sei();               // Enable global interrupts
		sleep_cpu();         // Enter sleep mode (wakes on interrupt)
		sleep_disable();     // Disable sleep mode after wake-up
		
		_delay_ms(10);       // Brief debounce delay
		
		// Check if the button is pressed
		if (!(PORTA.IN & (1 << 4))) {
			single_led_chaser_cw(2);
			_delay_ms(1000);
			single_led_chaser_ccw(2);
			all_leds_off();
			_delay_ms(500);
			twinkling_stars(15, 1000);
			
		}
	}
	
	return 0;
}

// Function to initialize all LED pins as outputs
void init_leds(void) 
{
	// Configure PORTA pins: PA3, PA2, PA1, PA7, PA6, PA5 as outputs
	PORTA.DIRSET = (1 << 3) | (1 << 2) | (1 << 1) | (1 << 7) | (1 << 6) | (1 << 5);
	
	// Configure PORTB pins: PB0, PB5 as outputs
	PORTB.DIRSET = (1 << 0) | (1 << 5);
}

// This functions turns a specific LED (D1 - D8) ON
void led_on(uint8_t led_number)
{
	if(led_number < 1 || led_number > 8)
	{
		return;
	}
	switch(led_number)
	{
		case 1: PORTA.OUTSET = (1 << 3); break;
		case 2: PORTA.OUTSET = (1 << 2); break;
		case 3: PORTA.OUTSET = (1 << 1); break;
		case 4: PORTB.OUTSET = (1 << 0); break;
		case 5: PORTB.OUTSET = (1 << 5); break;
		case 6: PORTA.OUTSET = (1 << 7); break;
		case 7: PORTA.OUTSET = (1 << 6); break;
		case 8: PORTA.OUTSET = (1 << 5); break;
	}
}

// This functions turns a specific LED (D1 - D8) OFF
void led_off(uint8_t led_number)
{
	if(led_number < 1 || led_number > 8)
	{
		return;
	}
	switch(led_number)
	{
		case 1: PORTA.OUTCLR = (1 << 3); break;
		case 2: PORTA.OUTCLR = (1 << 2); break;
		case 3: PORTA.OUTCLR = (1 << 1); break;
		case 4: PORTB.OUTCLR = (1 << 0); break;
		case 5: PORTB.OUTCLR = (1 << 5); break;
		case 6: PORTA.OUTCLR = (1 << 7); break;
		case 7: PORTA.OUTCLR = (1 << 6); break;
		case 8: PORTA.OUTCLR = (1 << 5); break;
	}
}

// Turns off all LEDs at once
void all_leds_off(void)
{
	PORTA.OUTCLR = (1 << 3) | (1 << 2) | (1 << 1) | (1 << 7) | (1 << 6) | (1 << 5);
	PORTB.OUTCLR = (1 << 0) | (1 << 5);
}

// Creates a small led "chaser". One LED is on and circling around the ornament clockwise
void single_led_chaser_cw(uint8_t number_of_cycles)
{
	for(uint8_t c = 0; c < number_of_cycles; c++)
	{
		for(uint8_t i = 1; i <= 8; i++)
		{
			all_leds_off();
			led_on(i);
			_delay_ms(CHASER_DELAY_TIME);
		}
	}
	all_leds_off();
}

// Function for single-LED chaser in a ring pattern (reverse direction)
void single_led_chaser_ccw(uint8_t number_of_cycles) 
{
	for (uint8_t c = 0; c < number_of_cycles; c++) {
		for (uint8_t i = 8; i >= 1; i--) {
			all_leds_off();       
			led_on(i);            
			_delay_ms(CHASER_DELAY_TIME);  
		}
	}
	all_leds_off();  
}

// Same as single_led_chaser() but with two LEDs instead of one.
void double_led_chaser(uint8_t number_of_cycles)
{
	for (uint8_t c = 0; c < number_of_cycles; c++) {
		for (uint8_t i = 1; i <= 8; i++) {
			all_leds_off();      
			led_on(i);           
			uint8_t next = (i % 8) + 1;  
			led_on(next);         
			_delay_ms(CHASER_DELAY_TIME);  
		}
	}
	all_leds_off();  // Ensure all off at end
}

// Revised twinkling stars function with variable delay support
void twinkling_stars(uint8_t cycles, uint16_t base_delay_ms) 
{
	srand(0xABCD);  // Seed random number generator 
	all_leds_off();
	for (uint8_t c = 0; c < cycles; c++) {
		uint8_t led = (rand() % 8) + 1;  // Select random LED from 1 to 8
		led_on(led);
		delay_millis(base_delay_ms + (rand() % 200));  // Variable delay
		led_off(led);
		_delay_ms(50);
	}
	all_leds_off();
}

void delay_millis(uint16_t ms)
{
	while(ms--)
	{
		_delay_ms(1);
	}
}