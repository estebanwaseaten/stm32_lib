#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>

/*  handler gets called whenever counter reaches zero
 *  counter counts clock cycles --> depends on clockSpeed
 *  when counter reaches zero --> interrupt is triggered and counter resets to RVR
 *  RVR is 24bits --> max 0xFFFFFF = 16777215
*/

void SYSTICK_handler( void );

static uint32_t g_ticks_per_us = 0;
static uint32_t g_ticks_per_ms = 0;
volatile uint32_t g_handlerCalledCount = 0;

void SYSTICK_enable( uint32_t clockSpeed )
{
    g_handlerCalledCount = 0;
    g_ticks_per_us = clockSpeed / 1000000;
    g_ticks_per_ms = clockSpeed / 1000;


    SETWRD( SYSTICK->RVR, 0xFFFFFF );     //set reset value --> only tick rarely (max count value)
    SETBIT( SYSTICK->CVR, 1 );                  //resets counter to 0

    SETBIT( SYSTICK->CSR, 2 );      //enable core clock as clock source
    SETBIT( SYSTICK->CSR, 1 );      //enable interrupt
    SETBIT( SYSTICK->CSR, 0 );      //enables counter

    //priority high?    CLRBITS
    CLRBITS(SCB->SHPR3, 0xFF, 24 );                // bits 31-24 SysTick
    //SETBITS(SCB->SHPR3, 0xFF, 24 );                // bits 31-24 SysTick
    setHandler_SysTick( SYSTICK_handler );		// called every ms
}

void SYSTICK_handler( void )
{
    g_handlerCalledCount++;
}

uint32_t SYSTICK_get_ticks_raw()
{
    return (uint32_t)(SYSTICK->CVR);
}

//somehow this does not work well
uint32_t SYSTICK_get_ms()
{
    return (uint32_t)( (SYSTICK->CVR + (g_handlerCalledCount * 0xFFFFFF) )/g_ticks_per_ms);
}

uint32_t SYSTICK_get_us()   //return counter value / 1000000
{
    return (uint32_t)( (SYSTICK->CVR + (g_handlerCalledCount * 0xFFFFFF) ) / g_ticks_per_us);
}
