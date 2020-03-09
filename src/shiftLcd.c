#include <avr/io.h>
#include <util/delay.h>
#include <shiftLcd.h>

// According to the datasheet we need to wait at least 40ms after power rises above
// 2.7v before sending commands.
void initialiseLcd( uint8_t data, uint8_t clock, uint8_t latch ) {
    _dataPin = data;
    _clockPin = clock;
    _latchPin = latch;

    // Set data, clock and latch pins as output
    DDR |= 1 << _dataPin | 1 << _clockPin | 1 << _latchPin;

    _displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    _numLines = 2;
    _currLine = 0;

    _delay_ms( 50 );

    // Clear the shift register
    shiftOut( 0x00 );
    shiftOut( 0x00 );

    // set 4 bit mode
    write4Bits( 0x03, 0 );
    _delay_ms( 5 );

    write4Bits( 0x03, 0 );
    _delay_ms( 5 );

    write4Bits( 0x03, 0 );
    _delay_us( 150 );

    write4Bits( 0x02, 0 );   // is this necessary?

    command( LCD_FUNCTIONSET | _displayfunction );
    _delay_ms(4.5);

    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON;  
    display();

    clear();

    // Initialize to default text direction (for romance languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    // set the entry mode
    command( LCD_ENTRYMODESET | _displaymode );
}

void display() {
    _displaycontrol |= LCD_DISPLAYON;
    command( LCD_DISPLAYCONTROL | _displaycontrol );
}

void clear() {
    command( LCD_CLEARDISPLAY );
    _delay_ms( 2 );
}

void shiftInit()
{
   //Make the Data(DS), Shift clock (SH_CP), Store Clock (ST_CP) lines output
   DDR |= (( 1 << _clockPin ) | ( 1 << _latchPin ) | ( 1 << _dataPin ));
}

//Sends a clock pulse on SH_CP line
void shiftPulse()
{
   //Pulse the Shift Clock
   PORT |= ( 1 << _clockPin );      //HIGH
   PORT &= ( ~( 1 << _clockPin ));  //LOW
}

//Sends a clock pulse on ST_CP line
void shiftLatch()
{
   //Pulse the Store Clock
   PORT |= ( 1 << _latchPin );      //HIGH
   _delay_loop_1( 1 );
   PORT &= ( ~( 1 << _latchPin ));  //LOW
   _delay_loop_1( 1 );
}

void print( uint8_t value ) {
    send( value, 1 );
}

void send( uint8_t value, uint8_t mode ) {
    write4Bits( value >> 4, mode );
    write4Bits( value, mode );
}

void command( uint8_t value ) {
    send( value, 0 );
}

void write( uint8_t value ) {
    send( value, 1 );
}

void write4Bits( uint8_t value, uint8_t mode ) {
    uint8_t cmd = 0;
    uint8_t data = 0;
    uint8_t EN_SWITCH = 0x02;

    if ( !mode ) {
        cmd = 0 | _backlight;
    } else {
        cmd = LCD_RS_PIN | _backlight;
    }

    data = value << 4 & 0xF0;
    
    // Set upper nibble of data to be sent, set enable clock high as well as the register select pin
    // data |= (( value << 4 ) & 0xF0 ) | ( 1 << LCD_EN_PIN ) | ( mode << LCD_RS_PIN );
    cmd |= EN_SWITCH;
    shiftOut( data | cmd );
    _delay_us( 1 );

    // Set enable clock low
    cmd &= ~EN_SWITCH;
    shiftOut( data );
    _delay_us( 1 );

    // Set enable clock high
    cmd |= EN_SWITCH;
    shiftOut( data );
    _delay_us( 100 );
}

void shiftOut( uint8_t data ) {
    PORTB &= ~( 1 << _latchPin );         // Set latch pin low
    for ( uint8_t i = 8; i; i-- )  {
      PORTB &= ~( 1<< _clockPin );       // Set clock pin low
      PORTB &= ~( 1<< _dataPin );     // Set data pin low

      if ( data & 0x01 ) {             // Most significant bit first
        PORTB |= 1 << _dataPin;      // Set data pin high
      }

      data >>= 1;                    // Left shift the received value with 1 bit
      PORTB |= 1<< _clockPin;          // Set clock pin high
    }
    PORTB |= 1<< _latchPin;            // Set latch pin high
}