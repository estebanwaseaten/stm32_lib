#include "../STM32.h"
#include "STM32F303.h"


void DAC_start_clock( uint32_t DACnum )
{
    //enable interface clocks:
    if( DACnum == 1 )
    {
        SETBIT( RCC->APB1ENR, 29 );
    }
    else if( DACnum == 2 )
    {
        SETBIT( RCC->APB1ENR, 26 );
    }

    (void)RCC->APB1ENR;             //read back
}


void DAC_enable( uint32_t channel )
{
    if( channel == 1 )
    {
        //simple prep without triggers and nothing...
        SETWRD( DAC->CR, 0x0 );
        SETBITS( DAC->CR, 0x7, 3 );    // channel 1 trigger to software
        SETBIT( DAC->CR, 0 );          // enable channel 1
    }
}

void DAC_set( uint16_t level )
{
    SETWRD( DAC->DHR12R1, level );
}

void DAC_disable( void )
{
    SETWRD( DAC->DHR12R1, 0x0 );
    SETWRD( DAC->CR, 0x0 );
}
