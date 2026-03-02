#include "../STM32.h"
#include "STM32F303.h"


bool initialized = false;

/************** GPIO functions
 *
 *
 */
void GPIO_init( void )
{
    if( initialized )
        return;

    //enable clocks
    SETBIT( RCC->AHBENR, 17 );      //set GPIOA enabled
    SETBIT( RCC->AHBENR, 18 );      //set GPIOB enabled

    (void)RCC->AHBENR;  // read-back to flush/ensure clock gate opened

    initialized = true;
    //__asm("nop");			            // execute one cycle (do nothing)
    //setWord( 0x20009000, RCC->AHBENR );
}

void GPIO_changeFunction( uint32_t pin, uint32_t function ) //not done yet
{
    //set mode for PIN to output:
    SETBITS( GPIOA->MODER, 0x1, pin*2 );
}

void GPIO_set( uint32_t pin )
{
    GPIOA->BSRR = ( 1 << pin );
}

void GPIO_unset( uint32_t pin )
{
    GPIOA->BSRR = ( 1 << ( pin + 16 ) );
}

int GPIO_get( uint32_t pin )
{
    return 0;
}
