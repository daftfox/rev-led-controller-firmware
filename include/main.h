#include <stdint.h>
// Pins
#define RX              PINB0   // USI DI
#define HC595_CLOCK     PINB4   // Shift register clock signal
#define HC595_LATCH     PINB3   // Shift register latch signal
#define HC595_SERIAL    PINB2   // Shift register data signal
#define LED_SERIAL      PINB1   // LED data pin

#define NUM_LEDS        60      // Number of LEDs on the strip
#define FRAMES_PER_SECOND  120

// Message constants
#define MSG_HEADER      0xDA    // Define message header '[' (binary value reversed due to nature of USI)
#define MSG_FOOTER      0xBA    // Define message footer ']' (binary value reversed due to nature of USI)

// Patterns
#define RAINBOW         0x52    // Define rainbow command parameter 'R'
#define KITT            0x4B    // Define KITT command parameter 'K'
#define SOLID           0x43    // Define solid color command parameter 'C'
#define PULSE           0x50    // Define pulse color command parameter 'P'
#define SPARKLE         0x53    // Define sparkle color command parameter 'S'
#define METEOR          0x4D    // Define meteor rain command parameter 'M'
#define STROBE          0x46    // Define strobe command parameter 'F'

// LEDs
CRGBArray<NUM_LEDS> leds;

// Flags
uint8_t _valueReceived;
boolean _msgHeaderReceived;
boolean _msgFooterReceived;

// Command array and position
uint8_t _msgCommand[ 4 ];
uint8_t _msgPos;

uint8_t reverseByte( uint8_t value );
void ledPatternRainbow();
void ledPatternKITT( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t eyeSize, uint8_t speedDelay, uint8_t returnDelay );
void leftToRight( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t eyeSize, uint8_t speedDelay, uint8_t returnDelay );
void rightToLeft( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t eyeSize, uint8_t speedDelay, uint8_t returnDelay );
void ledPatternSolid( uint8_t hue, uint8_t saturation, uint8_t value );
void ledPatternPulse( uint8_t hue, uint8_t saturation );
void executeCommand( uint8_t command[] );
void showStrip();
void rainbow( uint8_t hue );
void addGlitter( fract8 chanceOfGlitter );
void setPixel( uint8_t pixel, uint8_t hue, uint8_t saturation, uint8_t brightness );
void setAll( uint8_t hue, uint8_t saturation, uint8_t brightness );
void ledPatternSparkle( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t speedDelay );
void ledPatternMeteorRain( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t meteorSize, uint8_t meteorTrailDecay, boolean meteorRandomDecay, uint8_t speedDelay );
void ledPatternStrobe( uint8_t hue, uint8_t saturation, uint8_t value, uint8_t strobeCount, uint8_t flashDelay, int endPause );