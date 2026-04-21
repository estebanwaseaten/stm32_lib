#include "../STM32.h"
#include "STM32F303.h"

//assume there is only oine DAC

void DAC_start_clock( void )
{
    SETBIT( RCC->APB1ENR, 29 );
    (void)RCC->APB1ENR;             //read back
}

void DAC_enable( uint32_t channel )
{
    if( channel == 1 )
    {
        //no triggers --> ignore TSEL. values from  DAC_DHRx are set after 1 clock cycle
        //SETBITS( DAC->CR, 0x7, 3 );    // channel 1 trigger to software
        SETBIT( DAC->CR, 0 );          // enable channel 1
    }
    else if( channel == 2 )
    {
        //SETBITS( DAC->CR, 0x7, 19 );    // channel 2 trigger to software
        SETBIT( DAC->CR, 16 );          // enable channel 2
    }
}

void DAC_set( uint32_t channel, uint16_t level )
{
    if( channel == 1 )
    {
        SETWRD( DAC->DHR12R1, level );
    }
    else if( channel == 2 )
    {
        SETWRD( DAC->DHR12R2, level );
    }
}

void DAC_disable( uint32_t channel )
{
    if( channel == 1 )
    {
        SETWRD( DAC->DHR12R1, 0x0 );
        CLRBIT( DAC->CR, 0 );
    }
    else if( channel == 2 )
    {
        SETWRD( DAC->DHR12R2, 0x0 );
        CLRBIT( DAC->CR, 16 );
    }
}
