#include "../STM32.h"
#include "STM32F303.h"

void STMtest( void )
{
    RCC->CR |= (1 << 16);   //turn on HSE
    RCC->CFGR |= (1 << 0);  // HSE clock as main clck

    RCC->APB1ENR |= (1 << 29);  //enable DAC1 interface clock
    RCC->APB1ENR |= (1 << 26);  //enable DAC2 interface clock



    ADC_enable( 1 );

    for (int i = 0; i < 100; i++) {
        __asm("nop");
    }


    //write clock control register to RAM for debugging reasons
    setWord( 0x20000210, ADC1->CR );
    //read with sudo ./cheesecake -stmbinprint 0x20000210
}

void GPIOinit( void )
{
    RCC->AHBENR |= 0x00060000;
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

/************** ADC functions
 *
 *
 */
int ADC_enable( uint32_t num )
{
    //enable ADC clock for both ADCs
    RCC->AHBENR |= 0x30000000;
    __asm("nop");			//execute four cycles before
    __asm("nop");
    __asm("nop");
    __asm("nop");

    //turn on ADC voltage regulator:
    switch ( num )
    {
        case 1:
            if( !CHKBIT( ADC1->CR, 28 ) )
            {   //enable voltage regulator if not on.
                ADC1->CR &= 0xCFFFFFFF; //reset
                ADC1->CR = (1 << 28);   //enable
            }
            break;
        case 2:
            if( !CHKBIT( ADC2->CR, 28 ) )
            {   //enable voltage regulator if not on.
                ADC2->CR &= 0xCFFFFFFF; //reset
                ADC2->CR = (1 << 28);
            }
            break;
        case 3:
            if( !CHKBIT( ADC2->CR, 28 ) )
            {   //enable voltage regulator if not on.

                ADC3->CR &= 0xCFFFFFFF; //reset
                ADC3->CR = (1 << 28);
            }
            break;
        case 4:
            if( !CHKBIT( ADC2->CR, 28 ) )
            {   //enable voltage regulator if not on.

                ADC4->CR &= 0xCFFFFFFF; //reset
                ADC4->CR = (1 << 28);
            }
            break;
        default:
            return -1;
    }
    return 0;
}
