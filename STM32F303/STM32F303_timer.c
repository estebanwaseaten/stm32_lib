#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>


// not sure which clocks need to be running
// source can be APB1 (6,7)
// source can either be APB1 clock or sysClk (2,3,4)
// source can either be APB2 clock or sysClk (1,8,15,16,17,20)
void TIMER_init( void )
{
    //depends on timer what to enable here...
}

//interrupt is
void TIMER_enable( uint32_t tim, uint32_t divider, bool pllSrc ) //clock cycle divider
{
    //divider?
    //set clock source here:    RCC_CFGR3

    switch( tim )
    {
        case 1:
            if( !CHKBIT( RCC->APB2ENR, 11 ) )   //on APB1
            {
                SETBIT( RCC->APB2ENR, 11 );
                (void)RCC->APB2ENR;             // read-back to flush/ensure clock gate opened
            }     //enable TIM1 clock
            if( pllSrc )
                SETBIT( RCC->CFGR3, 8 );
            else
                CLRBIT( RCC->CFGR3, 8 );
            break;
        case 2:
            if( !CHKBIT( RCC->APB1ENR, 0 ) )   //on APB1
            {
                SETBIT( RCC->APB1ENR, 0 );
                (void)RCC->APB1ENR;             // read-back to flush/ensure clock gate opened
            }     //enable TIM2 clock
            if( pllSrc )
                SETBIT( RCC->CFGR3, 24 );
            else
                CLRBIT( RCC->CFGR3, 24 );
            break;
        case 3:
            if( !CHKBIT( RCC->APB1ENR, 1 ) )   //on APB1
            {
                SETBIT( RCC->APB1ENR, 1 );
                (void)RCC->APB1ENR;             // read-back to flush/ensure clock gate opened
            }     //enable TIM3 clock
            if( pllSrc )
                SETBIT( RCC->CFGR3, 25 );
            else
                CLRBIT( RCC->CFGR3, 25 );
            break;
        case 4:
            if( !CHKBIT( RCC->APB1ENR, 2 ) )   //on APB1
            {
                SETBIT( RCC->APB1ENR, 2 );
                (void)RCC->APB1ENR;             // read-back to flush/ensure clock gate opened
            }     //enable TIM4 clock
            if( pllSrc )
                SETBIT( RCC->CFGR3, 25 );   //same for TIM3 and TIM4
            else
                CLRBIT( RCC->CFGR3, 25 );
            break;
        case 8:
            if( !CHKBIT( RCC->APB2ENR, 13 ) )   //on APB2
            {
                SETBIT( RCC->APB2ENR, 13 );
                (void)RCC->APB2ENR;             // read-back to flush/ensure clock gate opened
            }     //enable TIM8 clock
            if( pllSrc )
                SETBIT( RCC->CFGR3, 9 );   //same for TIM3 and TIM4
            else
                CLRBIT( RCC->CFGR3, 9 );
            break;
        case 15:
            if( !CHKBIT( RCC->APB2ENR, 16 ) )   //on APB2
            {
                SETBIT( RCC->APB2ENR, 16 );
                (void)RCC->APB2ENR;             // read-back to flush/ensure clock gate opened
            }     //enable TIM15 clock
            if( pllSrc )
                SETBIT( RCC->CFGR3, 10 );   //same for TIM3 and TIM4
            else
                CLRBIT( RCC->CFGR3, 10 );
            break;
        case 16:
            if( !CHKBIT( RCC->APB2ENR, 17 ) )   //on APB2
            {
                SETBIT( RCC->APB2ENR, 17 );
                (void)RCC->APB2ENR;             // read-back to flush/ensure clock gate opened
            }     //enable TIM16 clock
            if( pllSrc )
                SETBIT( RCC->CFGR3, 11 );   //same for TIM3 and TIM4
            else
                CLRBIT( RCC->CFGR3, 11 );
            break;
        case 17:
            if( !CHKBIT( RCC->APB2ENR, 18 ) )   //on APB2
            {
                SETBIT( RCC->APB2ENR, 18 );
                (void)RCC->APB2ENR;             // read-back to flush/ensure clock gate opened
            }     //enable TIM17 clock
            if( pllSrc )
                SETBIT( RCC->CFGR3, 13 );   //same for TIM3 and TIM4
            else
                CLRBIT( RCC->CFGR3, 13 );
            break;
        case 20:
            if( !CHKBIT( RCC->APB2ENR, 20 ) )   //on APB2
            {
                SETBIT( RCC->APB2ENR, 20 );
                (void)RCC->APB2ENR;             // read-back to flush/ensure clock gate opened
            }     //enable TIM20 clock
            if( pllSrc )
                SETBIT( RCC->CFGR3, 15 );   //same for TIM3 and TIM4
            else
                CLRBIT( RCC->CFGR3, 15 );
            break;
    }
}

void TIMER_disable( uint32_t tim )
{

}
