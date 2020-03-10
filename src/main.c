#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <main.h>
#include <util.h>
#include <ledStrip.h>
#include <util/delay.h>

// Pin change interrupt detects start of UART reception.
ISR ( PCINT0_vect ) {
  if ( PINB & 1 << RX ) return;     // Ignore if RX is high
  GIMSK &= ~( 1 << PCIE );          // Disable pin change interrupts
  TCCR0A = 2 << WGM00;              // Timer in CTC mode
  TCCR0B = 2 << CS00;               // Set prescaler to /8
  OCR0A = 103;                      // Shift every (103+1)*8 cycles
  TCNT0 = 206;                      // Start counting from (256-52+2)

  // Enable USI OVF interrupt, and select Timer0 compare match as USI HC595_CLOCK source:
  USICR = 1 << USIOIE | 0 << USIWM0 | 1 << USICS0;
  USISR = 1 << USIOIF | 8;          // Clear USI OVF flag, and set counter
}

// USI overflow interrupt indicates we've received a byte
ISR ( USI_OVF_vect ) {
  USICR = 0;                        // Disable USI         
  _valueReceived = USIDR;           // Store received value

  if ( _valueReceived == MSG_HEADER ) {
    _msgHeaderReceived = 1;
  } else if( _valueReceived == MSG_FOOTER ) {
    _msgFooterReceived = 1;
  }

  GIFR = 1 << PCIF;                 // Clear pin change interrupt flag.
  GIMSK |= 1 << PCIE;               // Enable pin change interrupts again
}

void initialiseFlags() {
  _valueReceived = 0;
  _msgPos = 0;
  _msgHeaderReceived = 0;
  _msgFooterReceived = 0;
}

void clearFlags() {
  _msgPos = 0;
  _msgHeaderReceived = 0;
  _msgFooterReceived = 0;
}

void init() {
  // enable interrupts
  sei();

  initialiseFlags();
  initialiseLed();

  DDRB &= ~( 1 << RX );             // Set RX pin as input
  USICR = 0;                        // Disable USI.
  GIFR = 1 << PCIF;                 // Clear pin change interrupt flag.
  GIMSK |= 1 << PCIE;               // Enable pin change interrupts
  PCMSK |= 1 << PCINT0;             // Enable pin change on pin 0
}

// Main **********************************************
int main() {
  init();                                               // Initialise
  while ( 1 ) {
    if ( _valueReceived ) {                             // If we have received a new byte
      // Bytes are received in reverse order, so we have to reverse them
      uint8_t formattedValue = reverseByte( _valueReceived );

      if ( _msgPos == 5) {
        // Something went wrong, received command too long. Reset
        clearFlags();
        memset( _msgCommand, 0, _msgPos - 1 );
      } else if ( _msgFooterReceived ) {
        // Message footer received, execute command, clear command array and reset flags
        clearFlags();
        _valueReceived = 0;                             // Clear the received value
        executeLedPatternCommand( _msgCommand );        // Execute received command
        memset( _msgCommand, 0, _msgPos );   // Reset the contents of the command array
      } else if ( _msgHeaderReceived && _valueReceived != MSG_HEADER ) {
        // Message header received, append received value to command array
        _msgCommand[ _msgPos ] = formattedValue;        // Write the received value to the command array
        _msgPos++;                                      // Increment the position in the array
      }

      _valueReceived = 0;                               // Clear the received value
    }
    _delay_us(1);
  }
}