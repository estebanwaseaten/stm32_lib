#include "../STM32.h"
#include "STM32F303.h"



void COMP_start_clock( void )
{
    //no clock for COMP?
    SETBIT( RCC->APB2ENR, 0 );      //COMP and SYSCFG clock enable
    (void)RCC->APB2ENR;             //read back
}

void COMP_select_output( void )
{   //Bits 13:10 COMP1OUTSEL
    SETBITS( COMP->COMP1_CSR, 0xA, 10 );  // sets 1010 --> Timer 3 input capture 1  for gating the Timer
}

void COMP_select_input( void )
{
    SETBITS( COMP->COMP1_CSR, 0x4, 4 ); // sets 100: PA4 or DAC1 output if enabled --> DAC1 sets trigger level
}
