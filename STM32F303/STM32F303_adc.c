#include "../STM32.h"
#include "STM32F303.h"

uint8_t pllClkDiv[12] = {0,1,3,5,7,9,11,15,31,63,127,255};

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
        SETBIT( RCC->AHBENR, 28 );     //enable ADC12 interface clock
        (void)RCC->AHBENR;             // read-back to flush/ensure clock gate opened
        CLRWRD( ADC_common[1]->CCR );  //reset comon control register for ADC 1 and 2   --> selects PLL as clock source
    }
    else if( (ADCnum == 3) || (ADCnum == 4) )
    {
        SETBIT( RCC->AHBENR, 29 );      //enable ADC34 interface clock
        (void)RCC->AHBENR;              // read-back to flush/ensure clock gate opened
        CLRWRD( ADC_common[3]->CCR );   //reset comon control register for ADC 1 and 2   --> selects PLL as clock source
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

    SETBIT( ADC[1]->CFGR, 1 );             //DMACFG: 1: DMA Circular Mode selected  --> has to be set in COMMON->CCR for dual mode
    //SETBIT( ADC[1]->CFGR, 13 );            //CONT set - not needed because TIM2 should do the triggering

    //single mode (ignored in dual mode):
    SETBIT( ADC[1]->CFGR, 0 );             // enable DMA - in dual ADC mode ADC[x]->CCR: MDMA bits are relevant         ENABLE DMA
    //dual mode:
    SETBITS( ADC1_2_COMMON->CCR, 0x2, 14 );         //MDMA set to 0b10 = 12 and 10 bits
    SETBIT( ADC1_2_COMMON->CCR, 13 );               //DMA circular mode
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

    //1. set timer for period (timer runs at 144MHz)
    //2. make ADC params fit into that period (ADC runs at 72MHz maximum)

    //what is the ADC measurement period? minimum: 1us
    // conversion cycles for 12 bits: 12.5 clocks
    // sampling cycles SMPx:
    // 0x0 --> 1.5 clocks
    // 0x1 --> 2.5
    // 0x2 --> 4.5
    // 0x3 --> 7.5
    // 0x4 --> 19.5
    // 0x5 --> 61.5
    // 0x6 --> 181.5
    // 0x7 --> 601.5

    //fastest: 14 cycles at ADC at 144 MHz

    //   1 us --> 0x4 (19.6 clk cycles)     -- fast opamp buffer anyways???
    // > 1 us --> 0x5 (61.5 clk cycles)

    CLRWRD( ADC[1]->SMPR1 );              //fastest sample time
    SETBITS( ADC[1]->SMPR1, 0x4, 3 );     //slower sample time for channel 1 (61.5 ADC clock cycles) --> should work for 1 per us

    CLRWRD( ADC[2]->SMPR1 );              //fastest sample time
    SETBITS( ADC[2]->SMPR1, 0x4, 3 );     //slower sample time for channel 1 (61.5 ADC clock cycles) --> should work for 1 per us
}

uint32_t ADC12_maximize_sampling_time( uint32_t timArr, uint32_t timClk ) //needs timer value and timerSpeed
{
    uint32_t adcClk = ADC12_getClockHz();
    uint32_t ratio = timClk/adcClk; // always2 ??   we count timArr and need to figure out how many adcClk cycles fit into that...
    uint32_t cycles = ratio*(timClk * (timArr + 1u)) / (adcClk * 10u);

    setWord( 0x20009030, cycles );

    CLRWRD( ADC[1]->SMPR1 );              //fastest sample time
    CLRWRD( ADC[2]->SMPR1 );              //fastest sample time
    uint8_t value = 0;
    if( cycles > 614 )
    {
        value = 0x7;
    }
    else if( cycles > 194 )
    {
        value = 0x6;
    }
    else if( cycles > 74 )
    {
        value = 0x5;
    }
    else if( cycles > 32 )
    {
        value = 0x4;
    }
    else if( cycles > 20 )
    {
        value = 0x3;
    }
    else if( cycles > 17 )
    {
        value = 0x2;
    }
    else if( cycles > 15 )
    {
        value = 0x1;
    }
    else if( cycles > 14 )
    {
        value = 0x0;
    }
    SETBITS( ADC[1]->SMPR1, value, 3 );
    SETBITS( ADC[2]->SMPR1, value, 3 );
    //ONE ADC conversion is 12.5 ADC cycles + 1.5/2.5/4.5/7.5/19.5/61.5/181.5/601.5
    //                   or 13 + 14/15/17/20/32/74/194/614
    return value;
}

uint32_t ADC12_getClockHz( void )
{
    if( CHKBIT( ADC1_2_COMMON->CCR, 17 ) )
    {
        if( CHKBIT( ADC1_2_COMMON->CCR, 16 ) )  //11
        {
            return CLOCK_get_AHB()/4;
        }
        else    //10
        {
            return CLOCK_get_AHB()/2;
        }
    }
    else
    {
        if( CHKBIT( ADC1_2_COMMON->CCR, 16 ) )  //01
        {
            return CLOCK_get_AHB();
        }
        else    //00
        {
            uint32_t prescaler12 = (RCC->CFGR2 >> 4) & 0xF;
            if( prescaler12 > 11 )
            {
                return CLOCK_get_pllClk() / 256;
            }
            else
            {
                return CLOCK_get_pllClk() / (pllClkDiv[prescaler12] + 1);
            }
        }
    }
}

void ADC_enable_interrupt( uint32_t ADC_num )
{
    NVIC_ISER->BANK[0] |= (1u << 18);
}

inline void ADC1_watchdog_clear_and_disarm( void )
{
    SETBIT( ADC1->ISR, 7 );            // clear the watchdog flag
    CLRBIT( ADC1->IER, 7 );            // disable AWD1IE - no more watchdog interrupts
}

inline void ADC1_watchdog_arm( void )
{
    SETBIT( ADC1->IER, 7 );
}

void ADC_enable_watchdog( uint32_t ADCnum, uint16_t level )
{
    //turn off adc:
    ADC_enable_interrupt(1);
    //turn off ADC voltage regulator:
    switch ( ADCnum )
    {
        case 1:
            // Single(1) regular channel: AWD1SGL bit = 1, AWD1EN bit = 1, AWD1EN bit = 0

            SETBIT( ADC1->IER, 7 );      //enable interrupt

            SETBIT( ADC1->CFGR, 26 );    // select channel 1
            SETBIT( ADC1->CFGR, 23 );    // enable on regular channels
            SETBIT( ADC1->CFGR, 22 );    // enable a singular channel

            //                 0x0hhh0lll
            SETWRD( ADC1->TR1, 0x00FF0000 );       //threshold

            //(watchdogs 2&3 can be configured more flexibly)
            break;
        case 2:
            SETBIT( ADC2->IER, 7 );
            break;
        case 3:
            SETBIT( ADC3->IER, 7 );
            break;
        case 4:
            SETBIT( ADC4->IER, 7 );
            break;
        default:
            return;
    }
}

void ADC_disable_watchdogs()
{
    CLRBIT( ADC1->IER, 7 );
    CLRBIT( ADC2->IER, 7 );
    CLRBIT( ADC3->IER, 7 );
    CLRBIT( ADC4->IER, 7 );
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
