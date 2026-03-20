#include "../STM32.h"
#include "STM32F303.h"

#include <stddef.h>



//init selects and starts up clocks
void TIMER_init( uint32_t tim, bool pllSrc )
{                                          //1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19  20
    static const uint8_t clk_sel_bitmap[20] =      {8 , 24, 25, 25, 0 , 0 , 0 , 9 , 0 , 0 , 0 , 0 , 0 , 0 , 10, 11, 13, 0 , 0 , 15};
    static const uint8_t clk_reg[20] =             {2 , 1 , 1 , 1 , 0 , 1 , 1 , 1 , 0 , 0 , 0 , 0 , 0 , 0 , 2 , 2 , 2 , 0 , 0 , 2 };
    static volatile uint32_t* clk_addr_map[3] =    { 0, &RCC->APB1ENR, &RCC->APB2ENR };
    static const uint8_t clk_enable_bitmap[20] =   {11, 0 , 1 , 2 , 0 , 4 , 5 , 13, 0 , 0 , 0 , 0 , 0 , 0 , 16, 17, 18, 0 , 0 , 20};

    uint8_t clk_sel_bit = clk_sel_bitmap[ tim - 1 ];
    uint8_t clk_enable_bit = clk_enable_bitmap[ tim - 1 ];
    volatile uint32_t* clk_addr = clk_addr_map[ clk_reg[ tim - 1] ];        //needs to be a pointer!

    if( pllSrc )    //only writable under certain conditions (clock must be PLL and AHB/APB2 clocks max total division is 2)
    {
        SETBIT( RCC->CFGR3, clk_sel_bit );   // PLL vco output  -- tim2 --> 24 fine!
        //  timer clock rate by multiplied 2!!!
    }
    else
    {
        CLRBIT( RCC->CFGR3, clk_sel_bit );   // PCLK2
    }

    SETBIT( *clk_addr, clk_enable_bit );  //enable clock for TIM
    (void)RCC->APB2ENR;
    (void)RCC->APB1ENR;
}

void TIMER2_setup( uint32_t us_between )    // timer is supposed to fire a TRGO (master mode) every us_between...
{
    uint32_t clockRate = 0;
    if( CHKBIT( RCC->CFGR3, 24 ) ) //PLL
    {
        clockRate = 2 * CLOCK_get_pllClk(); //  timer clock rate by multiplied 2!!!
        //clockRate = CLOCK_get_pllClk();
    }
    else
    {
        clockRate = CLOCK_get_PCLK2();
    }

    setWord( 0x20009004, clockRate );   //correct clock rate 72000000 or 144000000

    // timer can tick at clock rate or slower
    // if this number is too big, we could always set the pre-scaler...
    uint32_t countTo = clockRate/1000000 * us_between; //(in us)
    setWord( 0x20009014, countTo );


    SETWRD( TIM2->CR1, 0x0 );   // re-init to default
    SETWRD( TIM2->CR2, 0 );     //
    SETWRD( TIM2->SMCR, 0 );    //slave mode control reg
    SETWRD( TIM2->DIER, 0 );    //reset interrupt reg

    SETWRD( TIM2->PSC, 0 );     // timer pre-scaler (divides clock frequency -> set to 0 -> no divide, timer ticks at clock frequency)

    SETBIT( TIM2->EGR, 0 );         //Re-initialize the counter and generates an update of the registers.
    SETWRD( TIM2->SR, 0 );          //clear pending flags UIF
    SETWRD( TIM2->ARR, countTo - 1U );   //
    SETBITS( TIM2->CR2, 0x2, 4 );   //update event creates TRGO --> needed to trigger ADC
    SETBIT( TIM2->CR1, 2 );         //only counter overflow/underflow generates an update

    setWord( 0x20009000, countTo );
}

void TIMER2_start()
{
    SETWRD( TIM2->SR, 0x0 );    // clear UIF
    SETWRD( TIM2->CNT, 0x0 );
    SETBIT( TIM2->CR1, 0 );     // enable counter
}

void TIMER2_stop()      //Immediately stop TIM2 (CEN=0) or clear EXTEN=0.
{
    CLRBIT( TIM2->CR1, 0 );     // disable counter
}

void TIMER2_resume()    //?
{
    SETBIT( TIM2->CR1, 0 );     // enable counter
}



void TIMER_enable_interrupt()
{
    SETBIT( TIM2->DIER, 0 );                     //Update interrupt enabled.
}

inline void TIMER2_clear_interrupt( void )      //
{
    SETWRD( TIM2->SR, 0 );
}

inline uint32_t TIMER2_getcount( void )
{
    setWord( 0x20009010, TIM2->SR );
    return TIM2->CNT;
}

void TIMER2_restart( uint32_t tim )
{
    switch( tim )
    {
        case 2:
            CLRBIT( TIM2->DIER, 0 );
            SETWRD( TIM2->CNT, 0x0 );
            SETBIT( TIM2->EGR, 0 );
            SETWRD( TIM2->SR, 0x0 );    // clear UIF
            SETBIT( TIM2->DIER, 0 );
            SETBIT( TIM2->CR1, 0 );     //enable counter
        break;
    }
}


void TIMER2_disable( uint32_t tim )
{
    CLRBIT( TIM2->CR1, 0 ); //stop counter
    uint32_t bank = (TIM2_IRQ >> 5);
    SETBIT( NVIC_ICPR->BANK[bank], TIM2_IRQ - bank*32); //clear pending

}




void testing( void )
{
    //depends on timer what to enable here...
    //testing:
    return;
    SETBIT( RCC->APB1ENR, 0 );  //enable TIM2 clock
    (void)RCC->APB1ENR;         //read back to delay until clock is active'

    //reset
    SETBIT( RCC->APB1RSTR, 0 );
    CLRBIT( RCC->APB1RSTR, 0 );

    // reset all
    SETWRD( TIM2->CR1, 0 );     //init to zero sets UDIS to 0 --> update event enabled
    SETWRD( TIM2->CR2, 0 );     //
    SETWRD( TIM2->SMCR, 0 );
    SETWRD( TIM2->DIER, 0 );

    uint32_t arr = 72000; // = clk * period; ---> triggers every period seconds... every ms --> 72000, every us --> 72
    //config
    SETWRD( TIM2->PSC, 0x0 );           //pre-scaler = 16 lower bits + 1     --> works fine --> every tick timer fires.
    SETWRD( TIM2->ARR, arr );           //triggers update event after counting to ARR
    SETWRD( TIM2->CNT, 0 );             // optional: start from 0

    SETBIT( TIM2->CR1, 2 );     //only counter overflow/underflow generates an update
    SETBIT( TIM2->EGR, 0 );     //reset update generation UG bit
    SETWRD( TIM2->SR, 0 );      //clear pending flags UIF

    //enable interrupt
    uint32_t bank = (TIM2_IRQ >> 5);
    SETBIT( NVIC_ICPR->BANK[bank], TIM2_IRQ - bank*32); //clear pending
    SETBIT( NVIC_ISER->BANK[bank], TIM2_IRQ - bank*32); //enable interrupt

    SETBIT( TIM2->DIER, 0 );    // Update interrupt enabled.
    SETBIT( TIM2->CR1, 0 );     //start counter

    /*

    One-pulse (auto-stop after one update):
    TIM2->CR1 |= TIM_CR1_OPM; // enable one-pulse mode
    TIM2->CNT = 0; TIM2->EGR = TIM_EGR_UG; TIM2->SR = 0;
    TIM2->DIER |= TIM_DIER_UIE; TIM2->CR1 |= TIM_CR1_CEN;

    */

}
