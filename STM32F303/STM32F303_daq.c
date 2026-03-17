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


//bool gHardwareTrigger;
bool gTriggerHappened;
uint32_t gLastTriggerEvent;		// record DMA pos of last trigger event. Check if there is enough datapoints after 1/2- and 1-DMA interrupt
uint32_t gDataBufferLength;
uint32_t gDataLength;			// --> circular buffer 200
uint32_t gHalfDataLength;

//settings to be set via cmds
uint32_t g_settings_triggerMode;		// 0 --> software, 1, 2, 3 or 4 hardware
uint32_t g_settings_triggerLevel;
uint32_t g_settings_triggerPos;

//settings that can be changed via SPI and define the DAQ
uint32_t g_settings_datapoints;			// fixed at 1024 (max)
uint32_t g_settings_adc_time;			// 000 = 1.5 cycles to 111 = 601,5 cycles per ADC - must be set in ADC SMPR1 register
uint32_t g_settings_acquisition_speed;	// how many datapoints per second (defines timer settings)
uint32_t g_settings_enabledChannels;

volatile uint32_t gDataReady;	//binary encoding 0b1111 for all four channels, 0b0001 for channel 1
volatile uint32_t gCurrentFetchChannel;
volatile uint32_t gDataStart;		//channel 1 & 2 --> 0x20000000, ...
volatile uint32_t gDataBitShift;		//channel1: 0 channel2: 16
volatile uint32_t gDataTransferSize;	//number of datapoints
volatile uint32_t gDataIndex; 			//points to current mem location during fast data transfer
volatile uint32_t gDataCounter;			//start with 0 and count up to gDataTransferSize
volatile uint32_t gDatapointsAcquired;

// called at 1/2 and full circular buffer --> opportunity to check if we have enough datapoints after trigger
void myDMA1_handler( void )
{
	// check if it is the DMA 1/2 or full interruptType
	if( CHKBIT(DMA1->ISR, 2 ) )		//Half transfer (HT) flag for channel 1
	{
		setWord( 0x20009038, getWord( 0x20009038 ) + 1 );	//debug
	}

	if( CHKBIT(DMA1->ISR, 1 ) )		//Transfer complete (TC) flag for channel 1
	{
		setWord( 0x2000903C, getWord( 0x2000903C ) + 1 );	//debug
		//depends on mode if data is ready or not?
		if( g_settings_triggerMode == TRIG_SOFTWARE )
		{
			gDataReady = g_settings_enabledChannels;
		}
	}

	DMA_clear_interrupts( 1 );

//	if( enoughDataPointsAfterTrigger )
//	{
		//stop timer
//	}

	// stop the timer!
	//Immediately stop TIM2 (CEN=0) or clear EXTEN=0.
	//debug

	//clear interrupt
	//DAQ12_stop();
	//DMA_clear_interrupts( 1 );

	//gDatapointsAcquired = gDataLength;
	//gDataReady = g_settings_enabledChannels;
	//setWord(0x20009034, gDataReady);

}

void myDMA2_handler( void )		//needed? later maybe
{
	//setWord( 0x20009038, getWord( 0x20009038 ) + 1 );
}

//thrown at hardware trigger event
void myADC_watchdog_handler( void )
{
	//uint32_t remaining = DMA1->CH[0].CNDTR;
	//awd_position = BUFFER_SIZE - remaining;  // ← Position im Buffer
    //awd_triggered = true;
}

void DAQ_currentFetchDone( void )
{
	//invalidate data for this channel	//gCurrentFetchChannel
	gDataCounter = 0;
	gDataIndex = 0;
	CLRBIT( gDataReady, gCurrentFetchChannel - 1 );
}

void DAQ_prepFetch( uint32_t channel )		//if
{
	gCurrentFetchChannel = channel;

	//start with first datapoint
	gDataStart = MEMBASE12;			// 0x20000000
	gDataBitShift = 0;

	//DMA1->CNDTR contains remaining number of data items to transfer in this circular run -->
	gDataCounter = 0;
	//SOFTWARE TRIGGER --> read from "current position in data buffer" - gDataLength;
	gDataIndex = (gDataBufferLength - DMA1->CH[0].CNDTR ) + 1;	//index is shifted by one because at element 0 the size of the array is stored
	if( gDataIndex > gDataLength )
	{
		gDataIndex -= gDataLength;
	}
	else
	{
	 	gDataIndex += (gDataBufferLength - gDataLength);
	}
	setWord( 0x20009030, gDataIndex );

	if( (channel == 3) || (channel == 4) )		//second set of ADCs (not yet implemented)
	{
		gDataStart = MEMBASE34;		//offset // 0x20008000
	}
	if( (channel == 2) || (channel == 4) )		//first set of ADCs
	{
		gDataBitShift = 16;
	}


	gDataTransferSize = g_settings_datapoints;
	setWord( MEMBASE12, gDataTransferSize + (gDataTransferSize << 16) );		//set length of remaing transfer into first bit.
}

uint32_t isDataAvailable( void )
{
	return gDataReady;
}

inline void setLastTriggerEvent( uint32_t dmaPos )
{
	gLastTriggerEvent = dmaPos;
}
inline uint32_t getLastTriggerEvent( void )
{
	return gLastTriggerEvent;
}


void DAQ_configDataBuffer( uint32_t newHalfData )		//
{
	// must be a power of 4
	uint32_t newBufferSize = 4*newHalfData;

	if( newBufferSize > MAXDATAPTS )
	{
		gDataBufferLength = MAXDATAPTS;
	}
	else
	{
		gDataBufferLength = newBufferSize;
	}

	gDataLength = gDataBufferLength/2;

	//reset DMA here and configure accordingly

}
//correct order
// 1. DMA config (NOT ENABLE!)
// 2. ADC config mit DMAEN
// 3. DMA enable
// 4. TIM2 configuration
//clocks need to be enabled before
void DAQ12_setup( void )
{
	//default starting values:
	//gHardwareTrigger = false;
	gTriggerHappened = false;
	gLastTriggerEvent = 0;				// record DMA pos of last trigger event. Check if there is enough datapoints after 1/2- and 1-DMA interrupt
	gDataBufferLength = 4*g_settings_datapoints;			// keep it short for testing
	gCurrentFetchChannel = 0;
	gDataLength = gDataBufferLength/2;
	gHalfDataLength = gHalfDataLength/4;

	setHandler_DMA( 1, 1, myDMA1_handler );
	//setHandler_DMA( 2, ..., myDMA2_handler );
	setHandler_ADC( 1, myADC_watchdog_handler );	//must still be enabled later
	setHandler_ADC( 2, myADC_watchdog_handler );	//must still be enabled later
	setHandler_ADC( 3, myADC_watchdog_handler );	//must still be enabled later
	setHandler_ADC( 4, myADC_watchdog_handler );	//must still be enabled later

    // init prepares everything in an arbitrary way
    // config sets specific

    //clocks and very general initialisation
	DMA_init( true, true );             //init both DMA channels
	ADC_init( 1 );			   // enables clocks and voltage regulator and does calibration
    ADC_init( 2 );
    //ADC_init( 3 );
    //ADC_init( 4 );

    TIMER_init( 2, true );		//init timer 2 with PLL

    //DMA circular or cont	--> depends on HW or SW Trigger

    //DMA setup:	(uses gDataLength)
    //DMA_setup( true );             //specific for this DAQ (should probaly map the ADC to channel and memory...)
	DMA_setup_peri( 1, 1, &ADC1_2_COMMON->CDR, (uint32_t*)0x20000004, 32, true, gDataBufferLength );
    //ADC setup:
    //ADC_setup( 0x3 );             //enables DMA and only afterwards DMA can be enabled
	ADC12_setup_dual();

	TIMER2_setup( 1 );	     //1 us between each acquisition --> 8000 acquisitions in 8ms

    // AFTER configuring ADC we can enable DMA otherwise there is a problem
	// do we need to enable more than one interrupt?
    DMA_enable_interrupt( 1, 1 );		//one interrupt per ADC dual pair
	DMA_enable( 1, 1 );
	//	DMA_enable( 2, 1 );	not needed in dual mode


	DAQ12_start();
}

void DAQ_change_mode( uint32_t newMode )
{

}

//changing timebase or buffer size --> always reset DMA and all variables so
void DAQ_changeTimebase( uint32_t timebase )
{
	//changer Timer2 settings (also maybe start from zero in the dma... --> dma needs to be reset too)
	//always check ADC is stopped and done before disabling DMA!!!

	//maybe also need to change ADC settings if the DAQ would be too slow?
	// optimize: DAQ as long as possible for given timebase

}

void DAQ_changeBufferSize( uint32_t datapoints )
{
	//change DMA settings --> gDataLength
	//stop timer
	//stop ADC //always check ADC is stopped and done before disabling DMA!!!
	//disable DMA
		//change DMA settings
	//enable DMA
	//enable ADC
	//restart timer
}

//after trigger event when the measuremen is done. stop timer:
void DAQ12_pause( void )
{
	TIMER2_stop();
}

//after data is read and resume command is issued. restart timer:
void DAQ12_resume( void )
{
	TIMER2_start();
}

// ADC1->DMA1CH1, (ADC2->DMA2CH1), ADC3->DMA2CH5, (ADC4->DMA2CH2) dual mode only uses the ADC-master DMA channels
void DAQ12_start( void )
{
    //reload DMA:
    DMA_reset( 1, 1 );     //reset DMA (especially the counter CNDTR)
	//DMA_reset( 2, 1 ); DMA2,1 is not used in DUAL mode

    SETBIT( ADC[1]->ISR, 4 );      // clear OVR flag just in case
	SETBIT( ADC[2]->ISR, 4 );      // clear OVR flag just in case
    SETBIT( ADC[1]->CR, 2 );       // ADC armed --> TRGO starts conversion
	SETBIT( ADC[2]->CR, 2 );       // ADC armed --> TRGO starts conversion

    TIMER2_start();		//works - always start timer LAST!!! (so all the ADCs start in sync)
}

void DAQ12_stop( void )
{
    TIMER2_stop();       //CLRBIT( TIM2->CR1, 0 );

    //stop ADC:
    SETBIT( ADC[1]->CR, 4 );       //ADSTP = 1
    while( CHKBIT( ADC[1]->CR, 4 ) )
    {
        __asm("nop");
    }
    SETBIT( ADC[1]->ISR, 4 );      //clear OVR flag just in case

	SETBIT( ADC[2]->CR, 4 );       //ADSTP = 1
    while( CHKBIT( ADC[2]->CR, 4 ) )
    {
        __asm("nop");
    }
	SETBIT( ADC[2]->ISR, 4 );

    //TCIF löschen
    //SETWRD( DMA1->IFCR, 0xFFFFFFFF );
}
