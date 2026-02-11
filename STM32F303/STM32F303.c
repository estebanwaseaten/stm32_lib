#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>


// initializes the clocks and waits until they are running
void CLOCK_init( void )
{
    // LSE clock (low speed): X2
    // HSI 8 MHz RC oscillator clock
    // HSE oscillator clock (external high-speed clock, either X3 on nucleo, or MCO from ST-LINK:
    //      MCO output of ST-LINK MCU is used as an input clock. This
    //      frequency cannot be changed, it is fixed at 8 MHz and connected to PF0/PD0/PH0-OSC_IN of the STM32 microcontroller.
    // PLL clock (either HSI or HSE can be used as source?)
    // after power on only HSI should be anabled

    SETBIT(RCC->CR, 16);     // turn on HSE    (needed for fast ADC)
    while( !CHKBIT(RCC->CR, 17) )
    {   //wait until clocks are running
        __asm("nop");
    }

    //settings for ADC and PLL dividers
    CLRWRD( RCC->CFGR2 );
    SETBIT( RCC->CFGR2, 13 );       //ADC34 PLL clock divided by 1
    SETBIT( RCC->CFGR2, 8 );        //ADC12 PLL clock divided by 1
    CLRBITS( RCC->CFGR2, 0xF, 0);    //HSE input to PLL not divided

    //settings for PLL mult and set PLL source
    CLRWRD( RCC->CFGR );
    SETBITS( RCC->CFGR, 0x7, 18 );    //set PLL mult to 0111 = x9 --> 72MHz
    SETBIT( RCC->CFGR, 16 );          //set PLL src to HSE

    //turn on PLL
    SETBIT( RCC->CR, 24 );           //turn on PLL
    while( !CHKBIT(RCC->CR, 25) )    //check if it is on
    {
        __asm("nop");
    }


    //set HSE as sysclock (does not matter)
    SETBIT( RCC->CFGR, 0 );           //set HSE as system clock
    //SETBIT( RCC->CFGR, 1 );           //set PLL as system clock --> need to use APB1 prescaler /2
    //SETBIT( RCC->CFGR, 10 );          //set APB1 prescaler to /2 DOUBLE CHECK!!
    while( !CHKBIT(RCC->CFGR, 2) )    //check if HSE is selected as system clock
    {
        __asm("nop");
    }
}
