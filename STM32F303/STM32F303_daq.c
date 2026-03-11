#include "../STM32.h"
#include "STM32F303.h"




// trigger via EXT which can be connected to internal timers...
// timer needs to be setup to acquire at certain rate
// EXTSEL = 0011 --> EXT3 (TIM2_CC2 event)
// EXTSEL = 1011 --> EXT11 (TIM2_TRGO event)
//, EXTEN
//setup DMA: ADC1 --> DMA1_CH1
//ADC2 --> DMA2_CH1
//ADC3 --> DMA2_CH5
//ADC4 --> DMA2_CH4

uint32_t gDataLength = 64;
uint32_t gMemBase = 0x20000004;


//correct order
// 1. DMA config (NOT ENABLE!)
// 2. ADC config mit DMAEN
// 3. DMA enable
// 4. TIM2 configuration
//clocks need to be enabled before
void DAQ1_setup( void )
{

    // init prepares everything in an arbitrary way
    // config sets specific

    //clocks and very general initialisation
	DMA_init( true, true );             //init both DMA channels
	ADC_init( 1 );			   // enables clocks and voltage regulator and does calibration
    ADC_init( 2 );
    ADC_init( 3 );
    ADC_init( 4 );

    TIMER_init( 2, true );		//init timer 2 with PLL


    //DMA circular or cont

    //DMA setup:
    DMA_setup();             //specific for this DAQ (should probaly map the ADC to channel and memory...)
    //ADC setup:
    ADC_setup();             //enables DMA and only afterwards DMA can be enabled
    TIMER2_setup( 1 );	     //1 us between each acquisition

    //AFTER configuring ADC we can enable DMA otherwise there is a problem
    DMA_enable_interrupt( 1, 1 );
	DMA_enable( 1, 1 );

	//TIMER

}


void DAQ1_start( void )
{
    //reload DMA:
    DMA_reset( 1, 1 );     //reset DMA (especially the counter CNDTR)

    SETBIT( ADC[1]->ISR, 4 );      // clear OVR flag just in case
    SETBIT( ADC[1]->CR, 2 );       // ADC armed --> TRGO starts conversion

    TIMER2_start();		//works
}

void DAQ1_stop( void )
{
    TIMER2_stop();       //CLRBIT( TIM2->CR1, 0 );

    //stop ADC:
    SETBIT( ADC[1]->CR, 4 );       //ADSTP = 1
    while( CHKBIT( ADC[1]->CR, 4 ) )
    {
        __asm("nop");
    }
    SETBIT( ADC[1]->ISR, 4 );      //clear OVR flag just in case

    //TCIF löschen
    //SETWRD( DMA1->IFCR, 0xFFFFFFFF );
}
