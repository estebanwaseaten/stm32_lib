#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>

//return int and function params int or uint32_t

volatile uint32_t g_sysClk = 0;
volatile uint32_t g_pllClk = 0;


// initializes the clocks and waits until they are running
void CLOCK_init( uint32_t sysClk )
{
    // LSE clock (low speed): X2 -- ignore for now  (precision timing)
    // HSI 8 MHz RC oscillator clock -- should always be running after power on
    // HSE oscillator clock (external high-speed clock, either X3 on nucleo, or MCO from ST-LINK:
    //  - MCO output of ST-LINK MCU is used as an input clock. This frequency cannot be changed, it is fixed at 8 MHz and connected to PF0/PD0/PH0-OSC_IN of the STM32 microcontroller.
    // PLL clock (either HSI or HSE can be used as source)

    //depending what is needed as sysClk

     g_sysClk = 0;
     g_pllClk = 0;

    if ( sysClk == SYSCLK_HSI )
    {
        CLOCK_start_HSI();
        g_sysClk = (uint32_t)(CLKSPEED_HSI * 1000000);  //MHz to Hz
    }
    else if ( sysClk == SYSCLK_HSE )
    {
        CLOCK_start_HSE();
        SETBIT( RCC->CFGR, 0 );           //set HSE as system clock
        while( !CHKBIT(RCC->CFGR, 2) ){ __asm("nop"); }    //check if HSE is selected as system clock
        g_sysClk = (uint32_t)(CLKSPEED_HSE * 1000000);  //MHz to Hz
    }
    else if ( sysClk == SYSCLK_PLL )   //use HSE by default
    {
        CLOCK_start_PLL( SYSCLK_HSE );      //start PLL based on HSE
        CLRBIT( RCC->CFGR, 13 );            //set APB2 (fast) prescaler to not divided
        SETBIT( RCC->CFGR, 10 );            //set APB1 (slow) prescaler to /2 DOUBLE CHECK!! --> max 36 MHz
        SETBIT( RCC->CFGR, 1 );             // set PLL as sys clock
        while( !CHKBIT(RCC->CFGR, 3) ){ __asm("nop"); }    //check if PLL is running as system clock
        g_sysClk = g_pllClk;  //MHz to Hz
    }
}

void CLOCK_enable_sysTick( uint32_t clockSpeed )
{
    SETWRD( SYSTICK->RVR, (uint32_t)(clockSpeed / 1000) );  //???       core clock speed / 1000
    SETBIT( SYSTICK->CVR, 1 );  //reset

    SETBIT( SYSTICK->CSR, 2 );      //enable core clock as clock source
    SETBIT( SYSTICK->CSR, 1 );      //enable interrupt
    SETBIT( SYSTICK->CSR, 0 );      //enable CSR
}

uint32_t CLOCK_get_sysClk( void )   //return main clock value in Hz.
{
    return g_sysClk;
}

uint32_t CLOCK_get_pllClk( void )   //return main clock value in Hz.
{
    return g_pllClk;
}

void CLOCK_start_HSI( void )
{
    if( CHKBIT(RCC->CR, 1) )   //HSI already running (should always be the case)
        return;

    SETBIT(RCC->CR, 0);
    while( !CHKBIT(RCC->CR, 1) ){ __asm("nop"); }   //wait until HSI clock is running
}

void CLOCK_start_HSE( void )
{
    if( CHKBIT(RCC->CR, 17) )   //HSE already running
        return;

    SETBIT(RCC->CR, 16);     // turn on HSE    (needed for fast ADC)
    while( !CHKBIT(RCC->CR, 17) ){ __asm("nop"); }   //wait until clocks are running
}

void CLOCK_start_PLL( uint32_t pllSource )  //for now assume max speed is wanted
{
    if( CHKBIT(RCC->CR, 25) )   //PLL already running
        return;

    CLRWRD( RCC->CFGR );
    if( pllSource == SYSCLK_HSI )
    {
        CLOCK_start_HSI();
        SETBITS( RCC->CFGR, 0x7, 18 );    //set PLL mult to 0111 = x9 --> 72MHz
        SETBIT( RCC->CFGR, 15 );          //set PLL src to HSI (STM32F303xD/E and STM32F398xE only)
        g_pllClk = (uint32_t)(CLKSPEED_HSI * 9 * 1000000);
    }
    else if( pllSource == SYSCLK_HSE )
    {
        CLOCK_start_HSE();
        SETBITS( RCC->CFGR, 0x7, 18 );    //set PLL mult to 0111 = x9 --> 72MHz
        SETBIT( RCC->CFGR, 16 );          //set PLL src to HSE (STM32F303xD/E and STM32F398xE only)
        g_pllClk = (uint32_t)(CLKSPEED_HSE * 9 * 1000000);
    }

    CLRWRD( RCC->CFGR2 );
    // more clock divider settings:
    CLRBITS( RCC->CFGR2, 0xF, 0);   //HSE input to PLL not divided
    SETBIT( RCC->CFGR2, 13 );       //ADC34 PLL clock divided by 1
    SETBIT( RCC->CFGR2, 8 );        //ADC12 PLL clock divided by 1

    //turn on PLL
    SETBIT( RCC->CR, 24 );           //turn on PLL
    while( !CHKBIT(RCC->CR, 25) ){ __asm("nop"); }    //check if it is on
}
