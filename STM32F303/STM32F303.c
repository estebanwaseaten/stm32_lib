#include "../STM32.h"
#include "STM32F303.h"


void GPIOinit( void )
{
    RCC->AHBENR = 0x00060000;
    __asm("nop");			//execute on cycle or so (do nothing)


}

void GPIOchangeFunction( uint32_t pin, uint32_t function )
{
    //set mode for PIN to output:
    SETBITS( GPIOA->MODER, 0x1, pin*2 );
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
