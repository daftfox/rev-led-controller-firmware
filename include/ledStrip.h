// Pins
#define LED_SERIAL  PINB1   // Define LED serial pin
#define NUM_LEDS    60      // Define number of LEDs

#define COLORLENGTH (NUM_LEDS/2)
#define FADE (256/COLORLENGTH)

// Patterns
#define RAINBOW     0x52    // Define rainbow command parameter 'R'
#define KITT        0x4B    // Define KITT command parameter 'K'
#define SOLID       0x53    // Define solid color command parameter 'S'
#define PULSE       0x50    // Define pulse color command parameter 'P'

void ledPatternRainbow();
void ledPatternKITT( uint8_t red, uint8_t green, uint8_t blue );
void ledPatternSolid( uint8_t red, uint8_t green, uint8_t blue );
void ledPatternPulse( uint8_t red, uint8_t green, uint8_t blue );
void ledInitialise();
void executeLedPatternCommand( uint8_t command[ 4 ] );