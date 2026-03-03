#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>


inline void TIMER_clear_interrupt( uint32_t tim )
{
    SETWRD( TIM2->SR, 0 );
}

inline uint32_t TIMER_getcount( uint32_t tim )
{
    setWord( 0x20009010, TIM2->SR );
    return TIM2->CNT;
}

// not sure which clocks need to be running
// source can be APB1 (6,7)
// source can either be APB1 clock or sysClk (2,3,4)
// source can either be APB2 clock or sysClk (1,8,15,16,17,20)
void TIMER_init( void )
{
    //depends on timer what to enable here...
    //testing:

    SETBIT( RCC->APB1ENR, 0 );  //enable TIM2 clock
    (void)RCC->APB1ENR;         //read back to delay until clock is active'

    //reset
    SETBIT( RCC->APB1RSTR, 0 );
    CLRBIT( RCC->APB1RSTR, 0 );

    // reset all
    SETWRD( TIM2->CR1, 0 );     //init to zero setz UDIS to 0 --> update event enabled
    SETWRD( TIM2->CR2, 0 );     //
    SETWRD( TIM2->SMCR, 0 );
    SETWRD( TIM2->DIER, 0 );

    //config
    SETWRD( TIM2->PSC, 0x1 );     //pre-scaler = 16 lower bits + 1     --> works fine
    SETWRD( TIM2->ARR, 0x20000000 );    //triggers update event after counting to ARR
    SETWRD( TIM2->CNT, 0 );              // optional: start from 0


    SETBIT( TIM2->EGR, 0 );     //reset update generation UG bit
    SETBIT( TIM2->CR1, 2 );     //only counter overflow/underflow generates an update
    SETWRD( TIM2->SR, 0 );      //clear pending flags UIF

    SETBIT( TIM2->DIER, 0 );    // Update interrupt enabled.

    //enable interrupt
    uint32_t bank = (TIM2_IRQ >> 5);
    SETBIT( NVIC_ICPR->BANK[bank], TIM2_IRQ - bank*32); //clear pending
    SETBIT( NVIC_ISER->BANK[bank], TIM2_IRQ - bank*32); //enable interrupt

    SETBIT( TIM2->CR1, 0 );     //start counter

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
            if( !CHKBIT( RCC->APB1ENR, 0 ) )    //on APB1
            {
                SETBIT( RCC->APB1ENR, 0 );      //enable TIM2 clock
                (void)RCC->APB1ENR;             // read-back to flush/ensure clock gate opened
            }
            if( pllSrc )
                SETBIT( RCC->CFGR3, 24 );
            else
                CLRBIT( RCC->CFGR3, 24 );

            SETBIT( TIM2->DIER, 0 );       //Update interrupt enabled.
            SETWRD( TIM2->PSC, 0 );
            SETWRD( TIM2->CR1, 0x0 );   // re-init to default
            SETBIT( TIM2->EGR, 0 );     //Re-initialize the counter and generates an update of the registers.
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

void TIMER_start( uint32_t tim )
{
    switch( tim )
    {
        case 2:
            SETWRD( TIM2->ARR, 0xF );   //count to 0xF
            SETWRD( TIM2->SR, 0x0 );    // clear UIF
            SETBIT( TIM2->CR1, 0 );     //enable counter
            //TIM2->CR2 has sth with DMA requests
        break;
    }

}

void TIMER_disable( uint32_t tim )
{
    CLRBIT( TIM2->CR1, 0 ); //stop counter
    uint32_t bank = (TIM2_IRQ >> 5);
    SETBIT( NVIC_ICPR->BANK[bank], TIM2_IRQ - bank*32); //clear pending

}
