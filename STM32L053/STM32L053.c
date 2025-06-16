#include "../STM32.h"
#include "STM32L053.h"


void STMtest( void )
{
    //setWord( 0x20000210, 0xF0F0F0F0 );
    setWord( 0x20001810, RCC->IOPENR );

    ADC_enable( 0 );


//    ADC_disable( 0 );       //stops program???
}

/************** CLOCK functions
 *
 *
 */
void CLOCK_init( void )     //enables all clocks...
{
    // HSI16 internal high speed clock setup
    RCC->CR &= ~(1U << 4 );      //do not divide the HSI16 output by /4

    // Turn on HSI16 (high speed internal)
    RCC->CR |= 1;
    while (!CHKBIT( RCC->CR, 2 ));     //this seems to work

    //enable HSE (high speed external)
    //RCC->CR |= (1U << 16);
    //while (!CHKBIT( RCC->CR, 17 ));

    // PLL clock multiplicator 16MHz x 2 = 32 MHz;
    RCC->CFGR |= (1U << 22 );    //set PLL output divider to /2
    RCC->CFGR |= (1U << 18 );    //set PLL output multiplicator to x4
    RCC->CFGR &= ~(1U << 16 );   //unset PLL source to use HSI16

    //Turn on PLL
    RCC->CR |= (1U << 24);
    while (!CHKBIT( RCC->CR, 25 ));     //this seems to work

    //switch to PLL as system clock
    RCC->CFGR |= (3U << 0 );    //---> blinks very fast now
}


/************** GPIO functions
 *
 *
 */
void GPIO_init( void )
{
    //enable clock:
    RCC->IOPENR |= 0x3;
    //enables A and B

    //bits 0-4 --> A, B, C, D, E
    //bit 7 --> H
}

void GPIO_changeFunction( uint32_t pin, uint32_t function )
{
    if( (pin == 13) || (pin == 14) )
        return; //kills communications because pins 13,14 are used for SW-DB

    // mask = (0x3 << 10); //inv mask = ~(0x3 << 2*pin)
    GPIOA->MODER &= ((~(0x3 << 2*pin)) | (1 << 2*pin));
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
    //could also check if output or input...
    return (GPIOA->IDR >> pin) & 1U;
}


/************** ADC functions
 *
 *
 */
int ADC_enable( uint32_t num )
{
    //enable ADC clock
    RCC->APB2ENR |= (1U << 9);

    //maybe first check if ADC clock is running?
    while( !CHKBIT(RCC->APB2ENR, 9) );


    //enable voltage regulator if not on (should not be necessary, because turns on when calibration is started):
    //if( !CHKBIT( ADC->CR, 28 ) )
    //{
    //    ADC->CR = (1U << 28);
    //}
    //setWord( 0x20001900, ADC->CR );     //this seems to be working


    //2. start calibration
    ADC->CR = (1U << 31);    //start calibration
    while( (ADC->CR >> 31) );


    __asm("nop");
    __asm("nop");
    setWord( 0x20001900, ADC->CALFACT );
    setWord( 0x20001910, ADC->CR );


    return 0;
}

int ADC_disable( uint32_t num )
{
    //disable voltage regulator if not off
    if( CHKBIT( ADC->CR, 28 ) )
    {
        ADC->CR &= ~(1U << 28);
    }

    //disable ADC clock
    RCC->APB2ENR &= ~(1U << 9);

    return 0;
}
