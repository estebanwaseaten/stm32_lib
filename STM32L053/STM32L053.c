#include "../STM32.h"
#include "STM32L053.h"

void STMtest( void )
{

}

/************** GPIO functions
 *
 *
 */
void GPIOinit( void )
{
    RCC->IOPENR |= 0x1;
    __asm("nop");
    //enables all, bits 0-4 --> A, B, C, D, E
    // bit 7 --> H
    volatile uint32_t *ptr = (uint32_t*)0x20000204;
	*ptr = RCC->IOPENR;
}

void GPIOchangeFunction( uint32_t pin, uint32_t function )
{
    SETBITS( GPIOA->MODER, 01, pin*2 );
}

void GPIOset( uint32_t pin )
{
    GPIOA->BSRR = ( 1 << pin );
}

void GPIOunset( uint32_t pin )
{
    GPIOA->BSRR = ( 1 << ( pin + 16 ) );
}

uint32_t GPIOget( uint32_t pin )
{
    return 0;
}


/************** ADC functions
 *
 *
 */
uint32_t ADCenable( uint32_t num )
{

}
