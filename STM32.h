//common header

#ifndef STM32_H
#define STM32_H

#include <stdint.h>

enum pin_functions
{
    PIN_OUTPUT,
    PIN_INPUT
};

#define SETBIT( reg, bit ) ((reg) |= (1U << (bit)))
#define SETBITS( reg, bits, shift ) ((reg) |= ((bits) << (shift)))
#define SETWRD( reg, word ) ((reg) |= (word))

#define CLRBIT( reg, bit ) ((reg) &= ~(1U << (bit)))
#define CLRBITS( reg, bits, shift ) ((reg) &= ~((bits) << (shift)))
#define CLRWRD( reg ) ((reg) &= 0x0)

#define CHKBIT( reg, bit ) ((reg) & (1U <<(bit)))

//MISC
void setWord( uint32_t addr, uint32_t word );
void STMtest( void );

//RCC (clock)
void CLOCK_init( void );

//GPIO
void GPIO_init( void );
void GPIO_changeFunction( uint32_t pin, uint32_t function );
void GPIO_set( uint32_t pin );
void GPIO_unset( uint32_t pin );
uint32_t GPIO_get( uint32_t pin );

//ADC
int ADC_enable( uint32_t ADCnum );
int ADC_disable( uint32_t ADCnum );

//SPI
int SPI_enable( uint32_t SPInum );
int SPI_disable( uint32_t SPInum );

#endif
