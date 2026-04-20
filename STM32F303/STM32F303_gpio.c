#include "../STM32.h"
#include "STM32F303.h"


bool initialized = false;

/************** GPIO functions
 *
 *
 */
void GPIO_start_clock( void )
{
    if( initialized )
        return;

    //enable clocks
    SETBIT( RCC->AHBENR, 17 );      //set GPIOA enabled
    SETBIT( RCC->AHBENR, 18 );      //set GPIOB enabled
    (void)RCC->AHBENR;              // read-back to flush/ensure clock gate opened

    initialized = true;
}

void GPIO_changeFunction( uint32_t bank, uint32_t pin, uint32_t function ) //not done yet
{
    // 0b00 input, 0b01 general purpose output, 0b10 alternate function, 0b11 analog mode
    switch( bank )
    {
        case GPIO_BANK_A:
            SETBITS( GPIOA->MODER, function, pin*2 );
            break;
        case GPIO_BANK_B:
            SETBITS( GPIOB->MODER, function, pin*2 );
            break;
        case GPIO_BANK_C:
            SETBITS( GPIOC->MODER, function, pin*2 );
            break;
        case GPIO_BANK_D:
            SETBITS( GPIOD->MODER, function, pin*2 );
            break;
        case GPIO_BANK_E:
            SETBITS( GPIOE->MODER, function, pin*2 );
            break;
        case GPIO_BANK_F:
            SETBITS( GPIOF->MODER, function, pin*2 );
            break;
        case GPIO_BANK_G:
            SETBITS( GPIOG->MODER, function, pin*2 );
            break;
        case GPIO_BANK_H:
            SETBITS( GPIOH->MODER, function, pin*2 );
            break;
    }

}

void GPIO_set( uint32_t pin )
{
    GPIOA->BSRR = ( 1 << pin );
}

void GPIO_unset( uint32_t pin )
{
    GPIOA->BSRR = ( 1 << ( pin + 16 ) );
}

uint32_t GPIO_get( uint32_t pin )
{
    return 0;
}
