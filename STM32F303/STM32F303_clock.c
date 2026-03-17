#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>

//return int and function params int or uint32_t

#define CLKSPEED_HSI 8000000UL  //8MHz RC oscillator clock
#define CLKSPEED_HSE 8000000UL  //8MHz ext osc clock





// initializes the clocks and waits until they are running
void CLOCK_init( uint32_t sysClk )
{
    // LSE clock (low speed): X2 -- ignore for now  (precision timing)
    // HSI 8 MHz RC oscillator clock -- should always be running after power on
    // HSE oscillator clock (external high-speed clock, either X3 on nucleo, or MCO from ST-LINK:
    //  - MCO output of ST-LINK MCU is used as an input clock. This frequency cannot be changed, it is fixed at 8 MHz and connected to PF0/PD0/PH0-OSC_IN of the STM32 microcontroller.
    // PLL clock (either HSI or HSE can be used as source)

    //depending what is needed as sysClk

    if ( sysClk == SYSCLK_HSI )
    {
        CLOCK_start_HSI();
    }
    else if ( sysClk == SYSCLK_HSE )
    {
        CLOCK_start_HSE();
        SETBIT( RCC->CFGR, 0 );           //set HSE as system clock
        while( !CHKBIT(RCC->CFGR, 2) ){ __asm("nop"); }    //check if HSE is selected as system clock
    }
    else if ( sysClk == SYSCLK_PLL )   //use HSE by default
    {
        CLOCK_start_PLL( SYSCLK_HSE );      //start PLL based on HSE
        CLRBIT( RCC->CFGR, 13 );            //set APB2 (fast) prescaler to not divided
        SETBIT( RCC->CFGR, 10 );            //set APB1 (slow) prescaler to /2 DOUBLE CHECK!! --> max 36 MHz
        SETBIT( RCC->CFGR, 1 );             // set PLL as sys clock
        while( !CHKBIT(RCC->CFGR, 3) ){ __asm("nop"); }    //check if PLL is running as system clock
    }
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
    }
    else if( pllSource == SYSCLK_HSE )
    {
        CLOCK_start_HSE();
        SETBITS( RCC->CFGR, 0x7, 18 );    //set PLL mult to 0111 = x9 --> 72MHz
        SETBIT( RCC->CFGR, 16 );          //set PLL src to HSE (STM32F303xD/E and STM32F398xE only)
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

uint32_t CLOCK_get_sysClk( void )   //return main clock value in Hz.
{
    uint32_t system_clock_source = (RCC->CFGR & 0xF) >> 2;

    switch( system_clock_source )
    {
        case 0:     //HSI
            return CLKSPEED_HSI;
            break;
        case 1:     //HSE
            return CLKSPEED_HSE;
            break;
        case 2:     //PLL
            return CLOCK_get_pllClk();
            break;
        default:
            return 0;
    }
}

uint32_t CLOCK_get_pllClk( void )   //return main clock value in Hz.
{
    uint32_t pll_mul_bin = ( RCC->CFGR >> 18 ) & 0xF;
    uint32_t pll_mul_factor = pll_mul_bin + 2U;
    if( pll_mul_factor > 16U )
    { pll_mul_factor = 16U; }

    uint32_t divider = 1U;
    uint32_t src_hz;
    //PLLSRC
    if( CHKBIT(RCC->CFGR, 16) ) //HSE PREDIV
    {
        //HSE PREDIV(1)
        divider = (RCC->CFGR2 & 0xFU) + 1U;
        src_hz = CLKSPEED_HSE;
    }
    else
    {
        if( CHKBIT(RCC->CFGR, 15) ) //HSI used as PREDIV1
        {
            divider = 1U;
            src_hz = CLKSPEED_HSI;
        }
        else    // HSI/2 used as PREDIV(1)
        {
            divider = 2U;
            src_hz = CLKSPEED_HSI;
        }
    }
    return (uint32_t)((src_hz / divider ) * pll_mul_factor);
}

uint32_t CLOCK_get_AHB( void )    //APB2 clock
{
    uint32_t ahb_prescaler_bin = ( RCC->CFGR >> 4 ) & 0x3;
    uint32_t ahb_prescaler_factor = 1;
    if( CHKBIT( RCC->CFGR, 7 ) )     //starts with 1x
    {
        if( CHKBIT( RCC->CFGR, 6 ) ) //starts with 11
        {
            ahb_prescaler_factor <<= ( ahb_prescaler_bin + 6 );
        }
        else                        //starts with 10
        {
            ahb_prescaler_factor <<= ( ahb_prescaler_bin + 1 );
        }
    }
    return CLOCK_get_sysClk()/ahb_prescaler_factor;
}

uint32_t CLOCK_get_PCLK1( void )    //APB2 clock
{
    uint32_t apb1_prescaler_bin = ( RCC->CFGR >> 8 ) & 0x3;
    uint32_t apb1_prescaler_factor = 1;
    if( CHKBIT( RCC->CFGR, 10 ) )
    {
        apb1_prescaler_factor <<= ( apb1_prescaler_bin + 1 );
    }
    return CLOCK_get_AHB()/apb1_prescaler_factor;
}

uint32_t CLOCK_get_PCLK2( void )    //APB2 clock
{
    uint32_t apb2_prescaler_bin = ( RCC->CFGR >> 11 ) & 0x3;
    uint32_t apb2_prescaler_factor = 1;
    if( CHKBIT( RCC->CFGR, 13 ) )
    {
        apb2_prescaler_factor <<= ( apb2_prescaler_bin + 1 );
    }
    return CLOCK_get_AHB()/apb2_prescaler_factor;
}
