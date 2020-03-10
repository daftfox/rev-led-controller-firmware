#include <stdint.h>

/* Reverses the order of bytes passed as an argument.
 * example:
 * uint8_t value = 0x0F; (0000 1111)
 * uint8_t reversedValue = reverseByte( value );
 * reversedValue => 0xF0 (1111 0000)
 */
unsigned char reverseByte ( uint8_t value ) {
  value = (( value >> 1 ) & 0x55 ) | (( value << 1 ) & 0xAA );
  value = (( value >> 2 ) & 0x33 ) | (( value << 2 ) & 0xCC );
  value = (( value >> 4 ) & 0x0F ) | (( value << 4 ) & 0xF0 );
  return value;    
}