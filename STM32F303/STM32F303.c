#include "../STM32.h"
#include "STM32F303.h"

//test
void STMtest( void )
{
    setWord( 0x20009000, 0 );
    setWord( 0x20009004, 0 );
    setWord( 0x20009008, 0 );
    setWord( 0x2000900C, 0 );

    //turn on HSE
    SETBIT( RCC->CR, 16 );           //turn on HSE
    while( !CHKBIT(RCC->CR, 17) )   //check if it is on
    {
        __asm("nop");
    }

    //settings for ADC and PLL dividers
    CLRWRD( RCC->CFGR2 );
    SETBIT( RCC->CFGR2, 13 );       //ADC34 PLL clock divided by 1
    SETBIT( RCC->CFGR2, 8 );        //ADC12 PLL clock divided by 1
    CLRBITS( RCC->CFGR2, 0xF, 0);    //HSE input to PLL not divided

    //settings for PLL mult and set PLL source
    CLRWRD( RCC->CFGR );
    SETBITS( RCC->CFGR, 0x7, 18 );    //set PLL mult to 0111 = x9 --> 72MHz
    SETBIT( RCC->CFGR, 16 );          //set PLL src to HSE

    //set HSE as sysclock (does not matter)
    SETBIT( RCC->CFGR, 0 );           //set HSE as system clock
    while( !CHKBIT(RCC->CFGR, 2) )    //check if HSE is selected as system clock
    {
        __asm("nop");
    }

    //turn on PLL
    SETBIT( RCC->CR, 24 );           //turn on PLL
    while( !CHKBIT(RCC->CR, 25) )    //check if it is on
    {
        __asm("nop");
    }

    //enable ADC clocks
    SETBIT( RCC->AHBENR, 29 );  //enable ADC34 interface clock
    SETBIT( RCC->AHBENR, 28 );  //enable ADC12 interface clock

    __asm("nop");			//execute four cycles before
    __asm("nop");
    __asm("nop");
    __asm("nop");

    CLRWRD( ADC1_2_COMMON->CCR );

    //enable ADC voltage regulator if not on.
    if( !CHKBIT( ADC1->CR, 28 ) )
    {
        CLRBITS( ADC1->CR, 0x3, 28 );   //reset ADVREGEN[1:0] = 00
        SETBIT( ADC1->CR, 28 );         //enable voltage regulator ADVREGEN[1:0] = 01
        for (int i = 0; i < 100000; i++)    //wait at least 10 us (T_ADCVREG_STUP)
        {
            __asm("nop");
        }
    }

    //enable ADC
    SETBIT( ADC1->CR, 0 );              // enable adc ADC1->CR |= 2; to disable;
    while( !CHKBIT( ADC1->ISR, 0 ) )    //ISR register bit 0 is the ready flag
    {
        __asm("nop");
    }

}



// initializes the clocks and waits until they are running
void CLOCK_init( void )
{
    // LSE clock (low speed): X2
    // HSI 8 MHz RC oscillator clock
    // HSE oscillator clock (external high-speed clock, either X3 on nucleo, or MCO from ST-LINK:
    //      MCO output of ST-LINK MCU is used as an input clock. This
    //      frequency cannot be changed, it is fixed at 8 MHz and connected to PF0/PD0/PH0-OSC_IN of the STM32 microcontroller.
    // PLL clock (either HSI or HSE can be used as source?)
    // after power on only HSI should be anabled


    //RCC->CR = 0x01050001;    //turn on PLL, HSE, HSI

    SETBIT(RCC->CR, 16);     // turn on HSE    (needed for fast ADC)
    while( !CHKBIT(RCC->CR, 17) )
    {   //wait until clocks are running
        __asm("nop");
    }

    // 25: PLLRDY, 17: HSERDY, 1: HSIRDY
/*    while( !CHKBIT(RCC->CR, 25) || !CHKBIT(RCC->CR, 17) || !CHKBIT(RCC->CR, 1))
    {   //wait until clocks are running
        __asm("nop");
    }*/
}

/************** GPIO functions
 *
 *
 */
void GPIO_init( void )
{
    RCC->AHBENR |= 0x00060000;          // set GPIOAEN (port A clock enable and port B clock enable)
    __asm("nop");			            // execute one cycle (do nothing)
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
 * AHB3 should be clocked
 */

int ADC_enable( uint32_t ADCnum )
{


    //enable ADC clock for both ADCs (bits 28 and 29 for ADC1/2 and ADC3/4) --> RCC clock up to 72MHz
    SETBIT( RCC->AHBENR, 29 );  //enable ADC34 interface clock
    SETBIT( RCC->AHBENR, 28 );  //enable ADC12 interface clock
    __asm("nop");			//execute four cycles before
    __asm("nop");
    __asm("nop");
    __asm("nop");

    //turn on ADC voltage regulator:
    switch ( ADCnum )
    {
        case 1:
            //set/select clock:
            //ADC1_2_COMMON->CCR &= 0x30000;  //unset bits 17&16  //use specific clock source up tp 72MHz CKMODE[1:0] of the ADCx_CCR register must be reset.
            //want async --> bits 16 and 17 are 0

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
            while( !CHKBIT( ADC1->ISR, 0 ) ) //ISR register bit 0 is the ready flag
            {
                //break;
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

/************** SPI functions
 *
 *
 */

int SPI_enable( uint32_t SPInum )
{



    return 0;
}

int SPI_disable( uint32_t SPInum )
{
    return 0;
}
