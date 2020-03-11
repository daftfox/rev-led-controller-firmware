#include <FastLED.h>
#include <main.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>

extern "C" {
  volatile unsigned long timer0_millis = 0; // FAKE FAKE FAKE!
}

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
    _msgHeaderReceived = true;
  } else if( _valueReceived == MSG_FOOTER ) {
    _msgFooterReceived = true;
  }

  GIFR = 1 << PCIF;                 // Clear pin change interrupt flag.
  GIMSK |= 1 << PCIE;               // Enable pin change interrupts again
}

void initialiseFlags() {
  _valueReceived = 0;
  _msgPos = 0;
  _msgHeaderReceived = false;
  _msgFooterReceived = false;
}

void initialise() {
  sei();                            // Enable interrupts

  // Initialise FastLED and tell it to use the LED_SERIAL pin
  FastLED.addLeds<WS2812B, LED_SERIAL, GRB>( leds, NUM_LEDS );
  setAll(0, 0, 0);                  // Turn off all LEDs

  initialiseFlags();                // Initialise command parsing flags

  DDRB &= ~( 1 << RX );             // Set RX pin as input
  USICR = 0;                        // Disable USI.
  GIFR = 1 << PCIF;                 // Clear pin change interrupt flag.
  GIMSK |= 1 << PCIE;               // Enable pin change interrupts
  PCMSK |= 1 << PCINT0;             // Enable pin change on pin 0
}

// Main **********************************************
int main() {
  initialise();                                               // Initialise
  while ( 1 ) {
    if ( _valueReceived ) {                             // If we have received a new byte
      // Bytes are received in reverse order, so we have to reverse them
      uint8_t formattedValue = reverseByte( _valueReceived );

      if ( _msgPos == 5) {
        // Something went wrong, received command too long. Reset
        initialiseFlags();
        memset( _msgCommand, 0, ( sizeof( _msgCommand ) / sizeof( uint8_t ) ) );
      } else if ( _msgFooterReceived ) {
        // Message footer received, execute command, clear command array and reset flags
        initialiseFlags();
        executeCommand( _msgCommand );                  // Execute received command
        memset( _msgCommand, 0, _msgPos );              // Reset the contents of the command array
      } else if ( _msgHeaderReceived && _valueReceived != MSG_HEADER ) {
        // Message header received, append received value to command array
        _msgCommand[ _msgPos ] = formattedValue;        // Write the received value to the command array
        _msgPos++;                                      // Increment the position in the array
        _valueReceived = 0;                             // Clear the received value
      }
    }
    _delay_us(1);
  }
}

void executeCommand( uint8_t command[] ) {
  switch( command[ 0 ] ) {
    case RAINBOW:
      ledPatternRainbow();
      break;
    case STROBE:
      ledPatternStrobe( command[ 1 ], command[ 2 ], command[ 3 ], 10, 50, 1000 );
      break;
    case METEOR:
      ledPatternMeteorRain( command[ 1 ], command[ 2 ], command[ 3 ], 10, 64, true, 30 );
      break;
    case SPARKLE:
      ledPatternSparkle( command[ 1 ], command[ 2 ], command[ 3 ], 0 );
      break;
    case KITT:
      ledPatternKITT( command[ 1 ], command[ 2 ], command[ 3 ], 3, 50, 50 );
      break;
    case SOLID:
      ledPatternSolid( command[ 1 ], command[ 2 ], command[ 3 ] );
      break;
    case PULSE:
      ledPatternPulse( command[ 1 ], command[ 2 ] );
      break;
  }
}

void ledPatternStrobe( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t strobeCount, uint8_t flashDelay, int endPause ){
  for( int j = 0; j < strobeCount; j++ ) {
    setAll( hue, saturation, value );
    delay( flashDelay );
    setAll( 0, 0, 0 );
    _delay_ms( flashDelay );
  }
 
 _delay_ms( endPause );
}

void ledPatternRainbow() {
  uint8_t hue = 0;
  while ( 1 ) {
    if ( _valueReceived ) {
        break;
    }

    rainbow( hue );
    addGlitter( 40 );
    EVERY_N_MILLISECONDS( 20 ) { hue++; }
    showStrip();
  }
}

void rainbow( uint8_t hue ) {
  fill_rainbow( leds, NUM_LEDS, hue, 5 );
}

void addGlitter( fract8 chanceOfGlitter ) {
    if( random8() < chanceOfGlitter) {
        leds[ random8( NUM_LEDS ) ] += CRGB::White;
    }
}

void ledPatternKITT( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t eyeSize, uint8_t speedDelay, uint8_t returnDelay ) {
  while ( 1 ) {
    if ( _valueReceived ) {
        break;
    }

    rightToLeft( hue, saturation, value, eyeSize, speedDelay, returnDelay );
    leftToRight( hue, saturation, value, eyeSize, speedDelay, returnDelay );
  }
}

// used by NewKITT
void leftToRight( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t eyeSize, uint8_t speedDelay, uint8_t returnDelay ) {
  for( uint8_t i = 0; i < NUM_LEDS - eyeSize - 2; i++ ) {
    setAll( 0, 0, 0 );
    setPixel( i, hue, saturation, value / 5 );
    for( uint8_t j = 1; j <= eyeSize; j++ ) {
      setPixel( i + j, hue, saturation, value );
    }
    setPixel( i + eyeSize + 1, hue, saturation, value / 5 );
    showStrip();
    _delay_ms( speedDelay );
  }
  _delay_ms( returnDelay );
}

// used by NewKITT
void rightToLeft( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t eyeSize, uint8_t speedDelay, uint8_t returnDelay) {
  for( uint8_t i = NUM_LEDS - eyeSize - 2; i > 0; i-- ) {
    setAll( 0, 0, 0 );
    setPixel( i, hue, saturation, value / 5 );
    for( uint8_t j = 1; j <= eyeSize; j++ ) {
      setPixel( i + j, hue, saturation, value );
    }
    setPixel( i + eyeSize + 1, hue, saturation, value / 5 );
    showStrip();
    _delay_ms( speedDelay );
  }
  _delay_ms( returnDelay );
}

void ledPatternSparkle( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t speedDelay ) {
  while ( 1 ) {
    if ( _valueReceived ) {
        break;
    }

    uint8_t led = random8( NUM_LEDS );
    setPixel( led, hue, saturation, value );
    showStrip();
    _delay_ms( speedDelay );
    setPixel( led, 0, 0, 0 );
  }
}

void ledPatternMeteorRain( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t meteorSize, uint8_t meteorTrailDecay, boolean meteorRandomDecay, uint8_t speedDelay ) {  
  while ( 1 ) {
    if ( _valueReceived ) {
        break;
    }

    setAll( 0, 0, 0 );
 
    for( uint8_t i = 0; i < NUM_LEDS + NUM_LEDS; i++ ) {
    
    
      // fade brightness all LEDs one step
      for( uint8_t j = 0; j < NUM_LEDS; j++ ) {
        if( ( !meteorRandomDecay ) || ( random8( 10 ) > 5 ) ) {
          leds[ j ].fadeToBlackBy( meteorTrailDecay );
        }
      }
    
      // draw meteor
      for( uint8_t j = 0; j < meteorSize; j++ ) {
        if( ( i - j < NUM_LEDS ) && ( i - j >= 0 ) ) {
          setPixel( i - j, hue, saturation, value );
        }
      }
    
      showStrip();
      _delay_ms( speedDelay );
    }
  }
}

void ledPatternSolid( uint8_t hue, uint8_t saturation, uint8_t value ) {
    setAll( hue, saturation, value );
}

void ledPatternPulse( uint8_t hue, uint8_t saturation ) {
  uint8_t brightness = 0;
  uint8_t pulseDelta = 1;
  while ( 1 ) {
    if ( _valueReceived ) {
        break;
    }

    for ( uint8_t i = 0; i < NUM_LEDS; i++ ) {
      setPixel( i, hue, saturation, ( uint8_t ) brightness );
    }

    if ( brightness == 0 ) {
      pulseDelta = 1;
    } else if ( brightness == 255 ) {
      pulseDelta = -1;
    }
  
    brightness += pulseDelta;
    showStrip();
  }
}

// Apply LED color changes
void showStrip() {
 FastLED.show();
}

// Set a LED color (not yet visible)
void setPixel( uint8_t pixel, uint8_t hue, uint8_t saturation, uint8_t brightness ) {
 leds[ pixel ] = CHSV( hue, saturation, brightness );
}

// Set all LEDs to a given color and apply it (visible)
void setAll( uint8_t hue, uint8_t saturation, uint8_t brightness ) {
  for( uint8_t i = 0; i < NUM_LEDS; i++ ) {
    setPixel( i, hue, saturation, brightness );
  }
  showStrip();
}

uint8_t reverseByte ( uint8_t value ) {
  value = (( value >> 1 ) & 0x55 ) | (( value << 1 ) & 0xAA );
  value = (( value >> 2 ) & 0x33 ) | (( value << 2 ) & 0xCC );
  value = (( value >> 4 ) & 0x0F ) | (( value << 4 ) & 0xF0 );
  return value;    
}