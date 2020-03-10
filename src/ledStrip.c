#include <ws2812Config.h>
#include <util/delay.h>
#include <main.h>
#include <ledStrip.h>
#include <light_ws2812.h>

struct cRGB colors[ 8 ];
struct cRGB led[ NUM_LEDS ];

void initialiseLed() {
    DDRB |= 1 << ws2812_pin;

    colors[ 0 ].r = 150; colors[ 0 ].g = 150; colors[ 0 ].b = 150;  // Faint white
    colors[ 1 ].r = 255; colors[ 1 ].g = 000; colors[ 1 ].b = 000;  // Red
    colors[ 2 ].r = 255; colors[ 2 ].g = 100; colors[ 2 ].b = 000;  // Orange
    colors[ 3 ].r = 100; colors[ 3 ].g = 255; colors[ 3 ].b = 000;  // Yellow
    colors[ 4 ].r = 000; colors[ 4 ].g = 255; colors[ 4 ].b = 000;  // Green
    colors[ 5 ].r = 000; colors[ 5 ].g = 100; colors[ 5 ].b = 255;  // Light blue (türkis)
    colors[ 6 ].r = 000; colors[ 6 ].g = 000; colors[ 6 ].b = 255;  // Blue
    colors[ 7 ].r = 100; colors[ 7 ].g = 000; colors[ 7 ].b = 255;  // Violet
};

void executeLedPatternCommand( uint8_t command[ 4 ] ) {
  switch( command[ 0 ] ) {
    case RAINBOW:
        ledPatternRainbow();
        break;
    case KITT:
        ledPatternKITT( command[ 1 ], command[ 2 ], command[ 3 ] );
        break;
    case SOLID:
        ledPatternSolid( command[ 1 ], command[ 2 ], command[ 3 ] );
        break;
    case PULSE:
        ledPatternPulse( command[ 1 ], command[ 2 ], command[ 3 ] );
        break;
  }
}

void ledPatternRainbow() {
    uint8_t k, j, i = 1;

    for ( i = NUM_LEDS; i > 0; i--) {
        led[ i - 1 ].r = 0;
        led[ i - 1 ].g = 0;
        led[ i - 1 ].b = 0;
    }

    while ( 1 ) {
        if ( _valueReceived ) {
            break;
        }

        uint8_t i = 0;
        for ( i = NUM_LEDS; i > 1; i--) {
            led[ i - 1 ] = led[ i - 2 ];
        }

        if ( k > COLORLENGTH) {
            j++;
            if( j > 7 )
            {
              j = 0;
            }

            k = 0;
        }
        k++;

        if( led[ 0 ].r < ( colors[ j ].r - FADE ) )
            led[ 0 ].r += FADE;
            
        if( led[ 0 ].r > ( colors[ j ].r + FADE ) )
            led[ 0 ].r -= FADE;

        if( led[ 0 ].g < ( colors[ j ].g - FADE ) )
            led[ 0 ].g += FADE;
            
        if( led[ 0 ].g > ( colors[ j ].g + FADE ) )
            led[ 0 ].g -= FADE;

        if( led[ 0 ].b < ( colors[ j ].b - FADE ) )
            led[ 0 ].b += FADE;
            
        if( led[ 0 ].b > ( colors[ j ].b + FADE ) )
            led[ 0 ].b -= FADE;

		_delay_ms(10);
        ws2812_sendarray( ( uint8_t * ) led, NUM_LEDS * 3 );
    }
}

// void addGlitter( fract8 chanceOfGlitter ) {
//     if( random8() < chanceOfGlitter) {
//         led[ random16(NUM_LEDS) ] += CRGB::White;
//     }
// }

void ledPatternKITT( uint8_t red, uint8_t green, uint8_t blue ) {
    while ( 1 ) {
        if ( _valueReceived ) {
            break;
        }
    }
}

void ledPatternSolid( uint8_t red, uint8_t green, uint8_t blue ) {
    for ( uint8_t i = 0; i < NUM_LEDS; i++ ) {
        led[ i ].r = red;
        led[ i ].g = green;
        led[ i ].b = blue;
    }

    ws2812_sendarray( ( uint8_t * ) &led, NUM_LEDS * 3 );
}

void ledPatternPulse( uint8_t red, uint8_t green, uint8_t blue ) {
    while ( 1 ) {
        if ( _valueReceived ) {
            break;
        }
    }
}