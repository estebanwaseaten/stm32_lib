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

void ADC_init( void )
{
    GPIO_init();    //just to make sure

    //enable ADC clock for both ADCs (bits 28 and 29 for ADC1/2 and ADC3/4) --> RCC clock up to 72MHz
    SETBIT( RCC->AHBENR, 29 );  //enable ADC34 interface clock
    SETBIT( RCC->AHBENR, 28 );  //enable ADC12 interface clock
    waitCycles( 4 );
}

int ADC_enable( uint32_t ADCnum )
{
    switch ( ADCnum )
    {
        case 1:
            //config input in analog mode:
            SETBITS( GPIOA->MODER, 0x3, 0*2 );      // PA0 alternate function (analog mode)
            SETBITS( GPIOA->MODER, 0x3, 1*2 );      // PA1 alternate function (analog mode)
            SETBITS( GPIOA->MODER, 0x3, 2*2 );      // PA2 alternate function (analog mode)

            CLRBITS( GPIOA->PUPDR, 0x3, 0*2 );      // no pull no push
            CLRBITS( GPIOA->PUPDR, 0x3, 1*2 );
            CLRBITS( GPIOA->PUPDR, 0x3, 2*2 );


            CLRWRD( ADC1_2_COMMON->CCR );

            //enable ADC voltage regulator if not on.
            if( !CHKBIT( ADC1->CR, 28 ) )
            {
                CLRBITS( ADC1->CR, 0x3, 28 );   //reset ADVREGEN[1:0] = 00
                SETBIT( ADC1->CR, 28 );         //enable voltage regulator ADVREGEN[1:0] = 01
                waitCycles(10000000);    //enough?

            }

            //ADD CALIBRATION:
        	CLRBIT( ADC1->CR, 30 );		// ADCALDIF=0 for single ended cal
        	//SETBIT( ADC1->CR, 30 );	// ADCALDIF=1 for differential cal
        	SETBIT( ADC1->CR, 31 ); 	// ADCAL=1 to start cal
        	// wait until ADCAL is zero again
        	while( CHKBIT( ADC1->CR, 31 ) )
            {
                __asm("nop");
            }
            waitCycles(4);

            //some stuff needs to be set before AD is enabled!!
            //SETBIT( ADC1->DIFSEL, 1 );
            CLRBIT( ADC1->DIFSEL, 1 );

            //enable ADC
            SETBIT( ADC1->CR, 0 );              // enable adc ADC1->CR |= 2; to disable;    "ADEN"
            while( !CHKBIT( ADC1->ISR, 0 ) )    //ISR register bit 0 is the ready flag
            {
                __asm("nop");
            }

            CLRBITS( ADC1->CFGR, 0x3, 10 );         //EXTEN = 0 --> software trigger by setting ADSTART
            CLRWRD( ADC1->SQR1 );                   //clear sequence -> sequence-length = 1 conversion
            SETBITS( ADC1->SQR1, 0x1 , 6 );         //select channel 1 for 1st conversion in regular sequence
            //SETBITS( ADC1->SQR1, 0x2 , 6 );       //select channel 2 for 1st conversion in regular sequence
            CLRWRD( ADC1->SQR2 );
            CLRWRD( ADC1->SQR3 );
            CLRWRD( ADC1->SQR4 );

            CLRWRD( ADC1->SMPR1 );              //fastest sample time
            SETBITS( ADC1->SMPR1, 0x3, 3 );     //slower sample time
            SETBITS( ADC1->SMPR1, 0x3, 6 );     //slower sample time

            CLRBIT( ADC1->CFGR, 16 );           // discontinuous mode for regular channels disabled DISCEN = 0
            CLRBIT( ADC1->CFGR, 13 );           // single conversion mode CONT = 0

            //start conversion with ADSTART=1: ADC1->CR |= 4;
            // The converted data are stored into the 16-bit ADCx_DR register
            // The EOC (end of regular conversion) flag is set
            // After the regular sequence is complete: The EOS (end of regular sequence) flag is set
            // To convert a single channel, program a sequence with a length of 1.

            break;
        case 2:
            ADC1_2_COMMON->CCR &= 0x30000;  //unset bits 17&16
            if( !CHKBIT( ADC2->CR, 28 ) )
            {   //enable voltage regulator if not on.
                ADC2->CR &= 0xCFFFFFFF; //reset
                ADC2->CR = (1 << 28);
            }
            break;
        case 3:
            ADC3_4_COMMON->CCR &= 0x30000;  //unset bits 17&16
            if( !CHKBIT( ADC2->CR, 28 ) )
            {   //enable voltage regulator if not on.

                ADC3->CR &= 0xCFFFFFFF; //reset
                ADC3->CR = (1 << 28);
            }
            break;
        case 4:
            ADC3_4_COMMON->CCR &= 0x30000;  //unset bits 17&16
            if( !CHKBIT( ADC2->CR, 28 ) )
            {   //enable voltage regulator if not on.

                ADC4->CR &= 0xCFFFFFFF; //reset
                ADC4->CR = (1 << 28);
            }
            break;
        default:
            return -1;
    }
    return 0;
}

uint16_t ADC_read_single( uint32_t ADCnum )
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
