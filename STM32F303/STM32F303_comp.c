#include "../STM32.h"
#include "STM32F303.h"



void COMP_start_clock( void )
{
    //no clock for COMP?
    SETBIT( RCC->APB2ENR, 0 );      //COMP and SYSCFG clock enable
    (void)RCC->APB2ENR;             //read back
}

void COMP_select_output( uint32_t num, uint32_t out )
{   //Bits 13:10 COMPxOUTSEL
    SETBITS( COMP[num-1].CSR, out, 10 );  // e.g. 0110 --> Timer 3 input capture 1  for gating the Timer
}

void COMP_select_input(  uint32_t num, uint32_t in  )
{   //Bits 6:4 COMPxINMSEL[2:0]:
    SETBITS( COMP[num-1].CSR, in, 4 ); // sets 100: PA4 or DAC1 output if enabled --> DAC1 sets trigger level
}
