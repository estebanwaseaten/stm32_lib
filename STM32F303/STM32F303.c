#include "../STM32.h"
#include "STM32F303.h"

//test



void STMtest( void )
{
    RCC->CR |= (1 << 16);   //turn on HSE
    RCC->CFGR |= (1 << 0);  // HSE CLOCK_inck as main clck

    RCC->APB1ENR |= (1 << 29);  //enable DAC1 interface clock
    RCC->APB1ENR |= (1 << 26);  //enable DAC2 interface clock



    ADC_enable( 1 );
    ADC_enable( 3 );

    for (int i = 0; i < 100; i++) {
        __asm("nop");
    }


    //write clock control register to RAM for debugging reasons
//    setWord( 0x20000210, ADC1->CR );
    //read with sudo ./cheesecake -stmbinprint 0x20000210
}


// initializes the clocks and waits until they are running
void CLOCK_init( void )
{
    RCC->CR = 0x01050001;    //turn on PLL, HSE, HSI
    while( !CHKBIT(RCC->CR, 25) || !CHKBIT(RCC->CR, 17) || !CHKBIT(RCC->CR, 1))
    {   //wait until clocks are running
        __asm("nop");
    }
}

/************** GPIO functions
 *
 *
 */

void GPIO_init( void )
{
    RCC->AHBENR |= 0x00060000;          // set GPIOAEN (port A clock enable and port B clock enable)
    __asm("nop");			            //execute on cycle or so (do nothing)
}

void GPIO_changeFunction( uint32_t pin, uint32_t function )
{
    //set mode for PIN to output:
    SETBITS( GPIOA->MODER, 0x1, pin*2 );
}

void GPIO_set( uint32_t pin )
{
    GPIOA->BSRR = ( 1 << pin );
}

void GPIO_unset( uint32_t pin )
{
    GPIOA->BSRR = ( 1 << ( pin + 16 ) );
}

uint32_t GPIO_get( uint32_t pin )
{
    return 0;
}


/************** ADC functions
 * 1. enable ADC internal voltage regulator (ADC voltage regulator enable sequence: a) change ADVREGEN[1:0] from 10 to 00 b) change from 00 to 01 )
 * 2. wait 10us for startup time
 * single ended vs differential:  DIFSEL[15:1]
 */
int ADC_enable( uint32_t ADCnum )
{
//    RCC->APB1ENR |= (1 << 29);  //enable DAC1 interface clock
//    RCC->APB1ENR |= (1 << 26);  //enable DAC2 interface clock

    RCC->APB1ENR |= (1 << 29);  //enable DAC1 interface clock
    RCC->APB1ENR |= (1 << 26);  //enable DAC2 interface clock


    //enable ADC clock for both ADCs (bits 28 and 29 for ADC1/2 and ADC3/4) --> RCC clock up to 72MHz
    RCC->AHBENR |= 0x30000000;
    __asm("nop");			//execute four cycles before
    __asm("nop");
    __asm("nop");
    __asm("nop");

    //turn on ADC voltage regulator:
    switch ( ADCnum )
    {
        case 1:
            //set/select clock:
            ADC1_2_COMMON->CCR &= 0x30000;  //unset bits 17&16  //use specific clock source up tp 72MHz CKMODE[1:0] of the ADCx_CCR register must be reset.

            // enable ADC internal voltage regulator & wait 10us
            if( !CHKBIT( ADC1->CR, 28 ) )
            {   //enable voltage regulator if not on.
                ADC1->CR &= 0x0;        //reset ADVREGEN[1:0] = 00
                ADC1->CR |= (1 << 28);  //enable voltage regulator ADVREGEN[1:0] = 01
                for (int i = 0; i < 100000; i++)    //wait at least 10 us (T_ADCVREG_STUP)
                {
                    __asm("nop");
                }
            }

            // enable ADC: set ADC1->CR:ADEN = 1 and wait till ready
            ADC1->CR |= 1;      // ADC1->CR |= 2; to disable;
            while( !CHKBIT( ADC1->ISR, 0 ) )
            {
                //not ready yet
                __asm("nop");
            }

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

//int ADC_read()

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
