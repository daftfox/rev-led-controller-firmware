#include <avr/io.h>
#include <avr/interrupt.h>
// #include <FastLED.h>
#include <shiftLcd.h>
#include <main.h>

/* ATtiny85 USI UART v3

   David Johnson-Davies - www.technoblogy.com - 6th May 2015
   and Edgar Bonet - www.edgar-bonet.org
   ATtiny85 @ 8MHz (external crystal; BOD disabled)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

// Constant
#define DataIn  PINB0         // USI DI
#define Clock   PINB1         // Shift register clock signal
#define Latch   PINB3         // Shift register latch signal
#define DataOut PINB2         // Shift register data signal

// USI UART **********************************************

unsigned char ReverseByte ( uint8_t x ) {
  x = (( x >> 1 ) & 0x55 ) | (( x << 1 ) & 0xAA );
  x = (( x >> 2 ) & 0x33 ) | (( x << 2 ) & 0xCC );
  x = (( x >> 4 ) & 0x0F ) | (( x << 4 ) & 0xF0 );
  return x;    
}

// Pin change interrupt detects start of UART reception.
ISR ( PCINT0_vect ) {
  if ( PINB & 1 << DataIn ) return;   // Ignore if DI is high
  GIMSK &= ~( 1 << PCIE );            // Disable pin change interrupts
  TCCR0A = 2 << WGM00;              // Timer in CTC mode
  TCCR0B = 2 << CS00;               // Set prescaler to /8
  OCR0A = 103;                    // Shift every (103+1)*8 cycles
  TCNT0 = 206;                    // Start counting from (256-52+2)
  // Enable USI OVF interrupt, and select Timer0 compare match as USI Clock source:
  USICR = 1 << USIOIE | 0 << USIWM0 | 1 << USICS0;
  USISR = 1 << USIOIF | 8;          // Clear USI OVF flag, and set counter
}

// USI overflow interrupt indicates we've received a byte
ISR ( USI_OVF_vect ) {
  USICR = 0;                      // Disable USI         
  int c = USIDR;
  print( ReverseByte( c ) );
  GIFR = 1 << PCIF;                 // Clear pin change interrupt flag.
  GIMSK |= 1 << PCIE;               // Enable pin change interrupts again
}

// Main **********************************************

void init() {
  sei();

  initialiseLcd( DataOut, Clock, Latch );

  DDRB &= ~( 1 << DataIn );       // Set DataIn pin to input
  USICR = 0;                      // Disable USI.
  GIFR = 1 << PCIF;               // Clear pin change interrupt flag.
  GIMSK |= 1 << PCIE;             // Enable pin change interrupts
  PCMSK |= 1 << PCINT0;           // Enable pin change on pin 0
}

int main() {
  init();
  while (1) {
    
  }
}