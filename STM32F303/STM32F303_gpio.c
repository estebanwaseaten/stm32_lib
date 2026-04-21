#include "../STM32.h"
#include "STM32F303.h"


bool initialized = false;

#define GPIO(bank) ((GPIO_map *)(GPIOA_REGS + (bank) * 0x400))

GPIO_map *GPIO_selectBank( uint32_t bank );


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

GPIO_map *GPIO_selectBank( uint32_t bank )
{
    if( bank > GPIO_BANK_H )
    {
        return NULL;
    }
    return GPIO( bank );
}

void GPIO_changeOutputType( uint32_t bank, uint32_t pin, uint32_t type )
{
    GPIO_map *selected = GPIO_selectBank( bank );

    type &= 0x1;    //mask the input not to overwrite other stuff
    CLRBITS( selected->OTYPER, 0x1, pin );
    SETBITS( selected->OTYPER, type, pin );
}

void GPIO_changeOutputSpeed( uint32_t bank, uint32_t pin, uint32_t speed )
{
    GPIO_map *selected = GPIO_selectBank( bank );
    speed &= 0x3;
    CLRBITS( selected->OSPEEDR, 0x3, pin*2 );
    SETBITS( selected->OSPEEDR, speed, pin*2 );
}

void GPIO_changeFunction( uint32_t bank, uint32_t pin, uint32_t function ) //not done yet
{
    GPIO_map *selected = GPIO_selectBank( bank );
    // 0b00 input, 0b01 general purpose output, 0b10 alternate function, 0b11 analog mode

    function &= 0x3;    //mask the input not to overwrite other stuff
    CLRBITS( selected->MODER, 0x3, pin*2 );
    SETBITS( selected->MODER, function, pin*2 );
}

void GPIO_changePull( uint32_t bank, uint32_t pin, uint32_t pull )
{
    GPIO_map *selected = GPIO_selectBank( bank );
    pull &= 0x3;
    CLRBITS( selected->PUPDR, 0x3, pin*2 );
    SETBITS( selected->PUPDR, pull, pin*2 );
}

void GPIO_altFunc( uint32_t bank, uint32_t pin, uint32_t function )
{
    GPIO_map *selected = GPIO_selectBank( bank );
    function &= 0xF;
    if( pin < 8 )
    {
        CLRBITS( selected->AFRL, 0xF, pin*4 );
        SETBITS( selected->AFRL, function, pin*4 );
    }
    else
    {
        CLRBITS( selected->AFRH, 0xF, (pin-8)*4 );
        SETBITS( selected->AFRH, function, (pin-8)*4 );
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
