//common header

#ifndef STM32_H
#define STM32_H

#include <stdint.h>

enum pin_functions
{
    PIN_OUTPUT,
    PIN_INPUT
};

#define SETWRD( reg, word ) ((reg) |= (word))
#define SETBITS( reg, bits, shift ) ((reg) |= ((bits) << (shift)))
#define SETBIT( reg, bit ) ((reg) |= (1U << (bit)))
#define CLRBIT( reg, bit ) ((reg) &= ~(1U << (bit)))
#define CHKBIT( reg, bit ) (((reg) >> (bit)) & 1U)

//MISC
void setWord( uint32_t addr, uint32_t word );
void STMtest( void );

//GPIO
void GPIOinit( void );
void GPIOchangeFunction( uint32_t pin, uint32_t function );
void GPIOset( uint32_t pin );
void GPIOunset( uint32_t pin );
uint32_t GPIOget( uint32_t pin );

//ADC
int ADC_enable( uint32_t num );





#endif
