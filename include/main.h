// Pins
#define RX              PINB0   // USI DI
#define HC595_CLOCK     PINB4   // Shift register clock signal
#define HC595_LATCH     PINB3   // Shift register latch signal
#define HC595_SERIAL    PINB2   // Shift register data signal

// Message constants
#define MSG_HEADER      0xDA    // Define message header '['
#define MSG_FOOTER      0xBA    // Define message footer ']'

// Flags
uint8_t _valueReceived;
uint8_t _msgHeaderReceived;
uint8_t _msgFooterReceived;

// Command array and position
uint8_t _msgCommand[ 4 ];
uint8_t _msgPos;

void clearFlags();