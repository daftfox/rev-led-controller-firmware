#include <stddef.h>

// Registers
#define PORT  PORTB
#define DDR   DDRB

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// pins for functional output
#define LCD_RS_PIN 0x01
#define LCD_EN_PIN 0x02
#define LCD_BL_PIN 0x04


void initialiseLcd( uint8_t data, uint8_t clock, uint8_t latch );
void write4Bits( uint8_t value, uint8_t mode );
void send( uint8_t value, uint8_t mode );
void shiftOut( uint8_t data );
void clear();
void display();
void command( uint8_t value );
void printCharacterToLcd( uint8_t value );
void printToLcd( uint8_t value[], size_t length );

uint8_t _dataPin;
uint8_t _clockPin;
uint8_t _latchPin;

uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;

uint8_t _backlight;

uint8_t _numLines, _currLine;