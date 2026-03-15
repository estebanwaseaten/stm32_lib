#include "../STM32.h"
#include "STM32F303.h"



//init for easier access
volatile ADC_map * const ADC[5] = {
    NULL,
    [1] = (volatile ADC_map *)ADC1_REGS,
    [2] = (volatile ADC_map *)ADC2_REGS,
    [3] = (volatile ADC_map *)ADC3_REGS,
    [4] = (volatile ADC_map *)ADC4_REGS
};
volatile ADC_common_map * const ADC_common[5] = {
    NULL,
    [1] = (volatile ADC_common_map *)ADC1_2_COMMON_REGS,
    [2] = (volatile ADC_common_map *)ADC1_2_COMMON_REGS,
    [3] = (volatile ADC_common_map *)ADC3_4_COMMON_REGS,
    [4] = (volatile ADC_common_map *)ADC3_4_COMMON_REGS
};

/************** ADC functions
 * 1. enable ADC internal voltage regulator (ADC voltage regulator enable sequence: a) change ADVREGEN[1:0] from 10 to 00 b) change from 00 to 01 )
 * 2. wait 10us for startup time
 * single ended vs differential:  DIFSEL[15:1]
 * AHB3 should be clocked
 */


void ADC_init( uint32_t ADCnum )
{
    GPIO_init();    //just to make sure

    //start ADC clocks
    if( (ADCnum == 1) || (ADCnum == 2) )
    {
        SETBIT( RCC->AHBENR, 28 );  //enable ADC12 interface clock
        (void)RCC->AHBENR;             // read-back to flush/ensure clock gate opened
        CLRWRD( ADC_common[1]->CCR );      //reset comon control register for ADC 1 and 2
    }
    else if( (ADCnum == 3) || (ADCnum == 4) )
    {
        SETBIT( RCC->AHBENR, 29 );  //enable ADC34 interface clock
        (void)RCC->AHBENR;             // read-back to flush/ensure clock gate opened
        CLRWRD( ADC_common[3]->CCR );      //reset comon control register for ADC 1 and 2
    }

    //start voltage regulator:
    if( !CHKBIT( ADC[ADCnum]->CR, 28 ) )
    {
        CLRBITS( ADC[ADCnum]->CR, 0x3, 28 );   //reset ADVREGEN[1:0] = 00
        SETBIT( ADC[ADCnum]->CR, 28 );         //enable voltage regulator ADVREGEN[1:0] = 01
        waitCycles(10000000);                  //enough?
    }

    //make sure it is off
    CLRBIT( ADC[ADCnum]->CR, 0 );
    (void)ADC[ADCnum]->CR;

    //calibrate:
    CLRBIT( ADC[ADCnum]->CR, 30 );	    // ADCALDIF=0 for single ended cal
    //	SETBIT( ADC1->CR, 30 );	        // ADCALDIF=1 for differential cal
    SETBIT( ADC[ADCnum]->CR, 31 ); 	    // ADCAL=1 to start cal
    // wait until ADCAL is zero again:
    while( CHKBIT( ADC[ADCnum]->CR, 31 ) )
    {
        __asm("nop");
    }
    waitCycles(4);
}

void ADC_setup( uint32_t ADCnum )
{

}

// ADC1 --> PA0 (ADC1_IN1)      (1-5 = fast channels)       // EXTSEL  = 1011 TIM2 reg          -- exists on nucleo board
// ADC2 --> PA4 (ADC2_IN1)      (1-5 = fast channels)       // EXTEN/EXTSEL only in master      -- exists on nucleo board
// ADC3 --> PB1 (ADC3_IN1)                                  // EXTSEL  = 0111 TIM2 reg          -- exists on nucleo board
// ADC4 --> PB12(ADC4_IN3)/PB14(ADC4_IN4)/PB15(ADC4_IN5)    // EXTEN/EXTSEL only in master      -- PB12, 14, 15 exist on nucleo board (righmost morpho connector)
// ADC 4: PE14, PE15, PB12, PB14, PB15
// DUAL ADC MODE: ADCx_CCR:DUAL --> regular simultaneous mode   //CONT, AUTDLY, DISCEN, DISCNUM[2:0], JDISCEN, JQM, JAUTO only need to be set in master
// read from ADC common data register though!!  --> set that in DMA!!!
// status from ADC common status reg ADCx_CSR... dual or not dual.
void ADC12_setup_dual( void )   //pin select?
{
    SETBITS( GPIOA->MODER, 0x3, 0*2 );      // PA0 alternate function (analog mode)
    CLRBITS( GPIOA->PUPDR, 0x3, 0*2 );      // no pull no push

    SETBITS( GPIOA->MODER, 0x3, 4*2 );      // PA4 alternate function (analog mode)
    CLRBITS( GPIOA->PUPDR, 0x3, 4*2 );      // no pull no push

    //some stuff needs to be set before ADC is enabled!!
    CLRBIT( ADC[1]->DIFSEL, 1 );       //differential mode off
    CLRBIT( ADC[1]->DIFSEL, 2 );       //differential mode off

    CLRBIT( ADC[2]->DIFSEL, 1 );       //differential mode off
    CLRBIT( ADC[2]->DIFSEL, 2 );       //differential mode off

    //enable ADC
    SETBIT( ADC[1]->CR, 0 );              // enable adc ADC1->CR |= 2; to disable;    "ADEN"
    while( !CHKBIT( ADC[1]->ISR, 0 ) )    //ISR register bit 0 is the ready flag
    {
        __asm("nop");
    }

    SETBIT( ADC[2]->CR, 0 );              // enable adc ADC1->CR |= 2; to disable;    "ADEN"
    while( !CHKBIT( ADC[2]->ISR, 0 ) )    //ISR register bit 0 is the ready flag
    {
        __asm("nop");
    }

    //setup of triggering scheme and DMA --> only for master
    CLRWRD( ADC[1]->CFGR );
    SETBIT( ADC[1]->CFGR, 10 );            //EXTEN -> hardware trigger detection on rising edge enabled
    SETBITS( ADC[1]->CFGR, 0xB, 6 );       //EXTSEL = 0b1011 = 0xB TIM2_TRGO event triggers conversion!                 SELECT TIMER TIM2

    //single mode (ignored in dual mode):
    SETBIT( ADC[1]->CFGR, 0 );             // enable DMA - in dual ADC mode ADC[x]->CCR: MDMA bits are relevant         ENABLE DMA
    //dual mode:
    SETBITS( ADC1_2_COMMON->CCR, 0x2, 14 );         //MDMA set to 0b10 = 12 and 10 bits
    CLRBIT( ADC1_2_COMMON->CCR, 13 );               //DMA one shot mode
    SETBITS( ADC1_2_COMMON->CCR, 0x6, 0 );          // regular simultaneous mode only


    CLRWRD( ADC[1]->SQR1 );                //clear sequence -> sequence-length = 1 conversion
    CLRWRD( ADC[1]->SQR2 );
    CLRWRD( ADC[1]->SQR3 );
    CLRWRD( ADC[1]->SQR4 );

    CLRWRD( ADC[2]->SQR1 );                //clear sequence -> sequence-length = 1 conversion
    CLRWRD( ADC[2]->SQR2 );
    CLRWRD( ADC[2]->SQR3 );
    CLRWRD( ADC[2]->SQR4 );

    SETBITS( ADC[1]->SQR1, 0x1 , 6 );         //select channel 1 for 1st conversion in regular sequence (do I need this?)
    SETBITS( ADC[2]->SQR1, 0x1 , 6 );         //select channel 1 for 1st conversion in regular sequence (do I need this?)

    CLRWRD( ADC[1]->SMPR1 );              //fastest sample time
    SETBITS( ADC[1]->SMPR1, 0x5, 3 );     //slower sample time for channel 1 (61.5 ADC clock cycles) --> should work for 1 per us

    CLRWRD( ADC[2]->SMPR1 );              //fastest sample time
    SETBITS( ADC[2]->SMPR1, 0x5, 3 );     //slower sample time for channel 1 (61.5 ADC clock cycles) --> should work for 1 per us
}







//not really needed anymore --> reading done via DMA
uint16_t ADC_read_single( uint32_t ADCnum )     //seems to read out the previous acquisition though...
{
    //Software starts ADC regular conversions by setting ADSTART=1; immediately: if EXTEN = 0x0 (software trigger)
    SETBIT( ADC1->CR, 2 );              //ADC start regular conversion  ADSTART

    while( !CHKBIT( ADC1->ISR, 3 ) )    //conversion not complete 3 = EOS, 2 = EOC
    {
        __asm("nop");
    }

    return (uint16_t)ADC1->DR;
}

// ADC_read_dma( uint32_t ADCnum, uint32_t datapoints ){}


int ADC_disable( uint32_t ADCnum )
{
    //turn off ADC voltage regulator:
    switch ( ADCnum )
    {
        case 1:
             // disable disable ADC1 by setting ADDIS:
            ADC1->CR |= 2;

            //disable voltage regulator:
            ADC1->CR &= 0xCFFFFFFF; //reset
            ADC1->CR = (1 << 29);   //disable
            break;
        case 2:
            ADC2->CR &= 0xCFFFFFFF; //reset
            ADC2->CR = (1 << 29);   //disable
            break;
        case 3:
            ADC3->CR &= 0xCFFFFFFF; //reset
            ADC3->CR = (1 << 29);   //disable
            break;
        case 4:
            ADC4->CR &= 0xCFFFFFFF; //reset
            ADC4->CR = (1 << 29);   //disable
            break;
        default:
            return -1;
    }

    RCC->AHBENR &= 0xCFFFFFFF;
    //disable ADC clock

// ORDER?

    //RCC->APB2ENR &= ~(1U << 9);

    return 0;
}
