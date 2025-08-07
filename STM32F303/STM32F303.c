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



void CLOCK_init( void )
{
    uint32_t counter = 0;
    setWord( 0x20006000, counter );

    RCC->CR = 0x01050001;    //turn on PLL, HSE, HSI
    while( !CHKBIT(RCC->CR, 25) || !CHKBIT(RCC->CR, 17) || !CHKBIT(RCC->CR, 1))
    {   //wait until clocks are running
        __asm("nop");
    }


    setWord( 0x20006000, counter );
}

/************** GPIO functions
 *
 *
 */

void GPIO_init( void )
{
    RCC->AHBENR |= 0x00060000;
    __asm("nop");			//execute on cycle or so (do nothing)
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
 *
 *
 */
int ADC_enable( uint32_t num )
{



    //enable ADC clock for both ADCs
    RCC->AHBENR |= 0x30000000;
    __asm("nop");			//execute four cycles before
    __asm("nop");
    __asm("nop");
    __asm("nop");

    //turn on ADC voltage regulator:
    switch ( num )
    {
        case 1:
            //set clock:
            //ADC1_2_COMMON->CCR->


            if( !CHKBIT( ADC1->CR, 28 ) )
            {   //enable voltage regulator if not on.
                ADC1->CR &= 0x0;        //reset
                ADC1->CR |= (1 << 28);   //enable voltage regulator
                for (int i = 0; i < 100000; i++)    //wait at least 10 us
                {
                    __asm("nop");
                }

                //ADC1->CR &= ~(1 << 30);     //clear ADCALDIF --> single input
                ADC1->CR = (1 << 31);      //start calibration     ALWAYS STUCK at 1
            }
            break;
        case 2:
            if( !CHKBIT( ADC2->CR, 28 ) )
            {   //enable voltage regulator if not on.
                ADC2->CR &= 0xCFFFFFFF; //reset
                ADC2->CR = (1 << 28);
            }
            break;
        case 3:
            if( !CHKBIT( ADC2->CR, 28 ) )
            {   //enable voltage regulator if not on.

                ADC3->CR &= 0xCFFFFFFF; //reset
                ADC3->CR = (1 << 28);
            }
            break;
        case 4:
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

int ADC_disable( uint32_t num )
{
    //turn off ADC voltage regulator:
    switch ( num )
    {
        case 1:
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

    //disable ADC clock
    RCC->APB2ENR &= ~(1U << 9);

    return 0;
}
