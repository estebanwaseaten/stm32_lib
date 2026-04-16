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


bool     gTriggerHappened;      //
uint32_t gLastTriggerEvent;		// record DMA pos of last trigger event. Check if there is enough datapoints after 1/2- and 1-DMA interrupt
bool     gRequestArmWatchdog;   //not needed soon ()

uint32_t gDataBufferLength;
uint32_t gDataLength;			// --> circular buffer 200
uint32_t gHalfDataLength;

// could create a settings struct?
//settings to be set via cmds
uint32_t g_settings_triggerMode;		// 0 --> software, 1, 2, 3 or 4 hardware
uint32_t g_settings_triggerLevel;
uint32_t g_settings_triggerPos;
uint32_t g_settings_triggerPosRemaining;
bool g_settings_triggerRisingEdge;


//settings that can be changed via SPI and define the DAQ
uint32_t g_settings_datapoints;			// fixed at 1024 (max)
uint32_t g_settings_adc_time;			// 000 = 1.5 cycles to 111 = 601,5 cycles per ADC - must be set in ADC SMPR1 register
uint32_t g_settings_acquisition_speed;	// how many datapoints per second (defines timer settings)
uint32_t g_settings_enabledChannels;

uint32_t gSetArrCounter;
uint32_t gARR;


volatile uint32_t gDataReady;	          //binary encoding 0b1111 for all four channels, 0b0001 for channel 1

//for fetching:
volatile uint32_t gCurrentFetchChannel;
volatile uint32_t gDataStart;		         //channel 1 & 2 --> 0x20000000, ...
volatile uint32_t gDataBitShift;		  //channel1: 0 channel2: 16
volatile uint32_t gDataTransferSize;	//number of datapoints
volatile uint32_t gDataIndex; 			//points to current mem location during fast data transfer
volatile uint32_t gDataCounter;			//start with 0 and count up to gDataTransferSize
volatile uint32_t gDatapointsAcquired;


//handlers:
// DMA1 and DMA2
// ADC watchdog (for triggering)


/********* DAQ interrupt handlers *********/
// called at 1/2 and full circular buffer --> opportunity to check if we have enough datapoints after trigger
void myDMA1_handler( void )
{
	// check if it is the DMA 1/2 or full interruptType
	if( CHKBIT(DMA1->ISR, 2 ) )		//Half transfer (HT) flag for channel 1
	{
		setWord( 0x20009038, getWord( 0x20009038 ) + 1 );	//debug

        if( g_settings_triggerMode != TRIG_SOFTWARE )
        {
            //currentDMApos = DMA_get_pos( 1, 1 );  --> current DMA position must be 1/2 total, so memory index is at gDataBufferLength/2 = gDataLength
            //check if enough datatpoints after trigger
            if( gLastTriggerEvent <= gDataLength )
            {
                if( (gDataLength - gLastTriggerEvent) >= g_settings_triggerPosRemaining )
                {
                    DAQ12_stop();
                    gDataReady = g_settings_enabledChannels;
                    gDataStart = gLastTriggerEvent;
                    setWord( 0x20009028, getWord( 0x20009028 ) + 1 );
                }
            }
            else    //the trigger happened at least one gDataLength before anyways!! (could be buggy if  the postion was exactly at 1/2?)
            {
                DAQ12_stop();
                gDataReady = g_settings_enabledChannels;
                gDataStart = gLastTriggerEvent;
                setWord( 0x20009028, getWord( 0x20009028 ) + 1 );
            }
        }
	}

	if( CHKBIT(DMA1->ISR, 1 ) )		//Transfer complete (TC) flag for channel 1
	{
		setWord( 0x2000903C, getWord( 0x2000903C ) + 1 );	//debug
		//depends on mode if data is ready or not?
        if( g_settings_triggerMode == TRIG_SOFTWARE )     //always ready after full DMA --> set gDataReady
		{
			gDataReady = g_settings_enabledChannels;
		}
        else
        {
            //currentDMApos = DMA_get_pos( 1, 1 );  --> current DMA position must be zero, so memory index is at gDataBufferLength
            //check if enough datatpoints after trigger = check if we are at least g_settings_triggerPosRemaining behind gLastTriggerEvent
            if( (gDataBufferLength - gLastTriggerEvent) >= g_settings_triggerPosRemaining ) //-->done
            {
                DAQ12_stop();
                gDataReady = g_settings_enabledChannels;     //need channels here!
                gDataStart = gLastTriggerEvent;
                setWord( 0x20009028, getWord( 0x20009028 ) + 1 );
            }

        }
	}
	DMA_clear_interrupts( 1 );

    //if trigger mode changed we can turn on the watchdog now (after at least a half filled buffer)
    if( gRequestArmWatchdog )
    {
        ADC1_watchdog_arm();
        gRequestArmWatchdog = false;
    }
}

void myDMA2_handler( void )		//needed? later maybe
{
	//setWord( 0x20009038, getWord( 0x20009038 ) + 1 );
}


//thrown at hardware trigger event
void myADC_watchdog_handler( void )     //not triggering currently
{
    // 1. save current DMA position
    gLastTriggerEvent = DMA_get_pos( 1, 1 );

    // 2. disable watchdog
    ADC1_watchdog_clear_and_disarm();  //until the data is acquired: no more triggers:

    //set trriggered flag:
    gTriggerHappened = true;

    //debug:
    setWord( 0x20009024, gLastTriggerEvent );
}


/********* DAQ data fetch *********/
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



/********* DAQ triggering *********/
inline void setLastTriggerEvent( uint32_t dmaPos )
{
	gLastTriggerEvent = dmaPos;
}
inline uint32_t getLastTriggerEvent( void )
{
	return gLastTriggerEvent;
}




//		correct order
// 1. DMA config (NOT ENABLE!)
// 2. ADC config mit DMAEN
// 3. DMA enable
// 4. TIM2 configuration
//clocks need to be enabled before
void DAQ12_setup( void )
{
	//default starting values:
	//gHardwareTrigger = false;
    gRequestArmWatchdog = false;
	gTriggerHappened = false;
	gLastTriggerEvent = 0;				// record DMA pos of last trigger event. Check if there is enough datapoints after 1/2- and 1-DMA interrupt
	gCurrentFetchChannel = 0;

	DAQ_config_DataBuffer( g_settings_datapoints, false );	//no dma reset here

	gSetArrCounter = 0;
	gARR = 143;	//1us period

	/* enable interrup handlers for
 	 *		- DMA transfers (to check if we acquired enough datapoints after trigger event or in general)
	 *		- ADC watchdog (to)
	 */
	setHandler_DMA( 1, 1, myDMA1_handler );
	//setHandler_DMA( 2, ..., myDMA2_handler );
	setHandler_ADC( 1, myADC_watchdog_handler );	// handler for ADC1
	setHandler_ADC( 2, myADC_watchdog_handler );	// handler for ADC2
	setHandler_ADC( 3, myADC_watchdog_handler );	// handler for ADC3
	setHandler_ADC( 4, myADC_watchdog_handler );	// handler for ADC4

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

	TIMER2_setup( gARR );	     //1 us between each acquisition --> 8000 acquisitions in 8ms

    // AFTER configuring ADC we can enable DMA otherwise there is a problem
		// do we need to enable more than one interrupt?
    DMA_enable_interrupt( 1, 1 );		//one interrupt per ADC dual pair
	DMA_enable( 1, 1 );
		//	DMA_enable( 2, 1 );	not needed in dual mode

	DAQ12_start();
}


/* Functions that start/stop/pause the DAQ
 */


//should this always also reset watchdogs etc...

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



/* Functions that change DAQ settings
 */

void DAQ_config_trigger_mode( uint16_t newMode )
{
	g_settings_triggerMode = newMode;

	if( newMode > 0 )
	{
		//void DMA_setup_peri( uint32_t dma, uint32_t chan, volatile uint32_t *sourceAddr, uint32_t *destAddr, uint32_t bitSize, bool circ, uint32_t bufferLength )
		//DMA_setup_peri( 1, 1, &ADC1_2_COMMON->CDR, (uint32_t*)0x20000004, 32, true, gDataBufferLength )

		//turn on watchdog
		//1. stop adc
		DAQ12_stop();

		//2. turn on watchdog
		//set AWDxIE  in ADCx_IER reg:
        ADC_enable_watchdog( 1, 1 );            // do this here when the ADC is off...
        ADC1_watchdog_clear_and_disarm();       // disabled now still, because we are just starting to fill the buffer (DMA is reset in DAQ12_start)
        gRequestArmWatchdog = true;

        //save current DMA pos: (for debugging now)
        setWord( 0x20009034, DMA_get_pos( 1, 1 ) );

		DAQ12_start();
	}
    else
    {

        DAQ12_stop();
        ADC_disable_watchdogs();
        DAQ12_start();
    }

}



void DAQ_config_trigger_level( uint16_t newLevel )
{
	g_settings_triggerLevel = newLevel;
}

void DAQ_config_trigger_pos( uint16_t newPos )
{
    if( newPos > gDataLength )
    {
        newPos = gDataLength;
    }
    g_settings_triggerPos = newPos;
    g_settings_triggerPosRemaining = gDataLength - g_settings_triggerPos - 1;   //-1?
}

void DAQ_config_trigger_risingEdge( bool rising )
{
    g_settings_triggerRisingEdge = rising;
}

uint16_t DAQ_config_trigger_mode_get( void )
{
    return g_settings_triggerMode;
}

uint16_t DAQ_config_trigger_level_get( void )
{
    return g_settings_triggerLevel;
}

uint16_t DAQ_config_trigger_pos_get( void )
{
    return g_settings_triggerPos;
}

bool DAQ_config_trigger_risingEdge_get( void )
{
    return g_settings_triggerRisingEdge;
}


void DAQ_config_ARR( uint8_t arrByte )		//set counter byte by byte
{
	uint32_t arr = arrByte;
	DAQ12_stop();
	switch( gSetArrCounter )
	{
		case 0:
			gARR = (arr << 24 ) & 0xFF000000;
			setWord( 0x20009020, gARR );
			gSetArrCounter++;
			break;
		case 1:
			gARR |= (arr << 16) & 0x00FF0000;
			setWord( 0x20009020, gARR );
			gSetArrCounter++;
			break;
		case 2:
			gARR |= (arr << 8 ) & 0x0000FF00;
			setWord( 0x20009020, gARR );
			gSetArrCounter++;
			break;
		case 3:
			gARR |= (arr & 0x000000FF);
			ADC12_maximize_sampling_time( gARR, TIMER2_getClockHz() );
			TIMER2_setup( gARR );
			setWord( 0x20009020, gARR );
			DAQ12_start();
			gSetArrCounter = 0;
			gARR = 0;
			break;
		default:
			gSetArrCounter = 0;
			break;
	}
}

uint32_t DAQ_config_DataBuffer( uint32_t dataPoints, bool dmaReset )		//half the data size --> guarantees multiple of 4
{
	if( CHKBIT( dataPoints, 0 ) )	//odd --> add one --> even
		dataPoints++;

	if( dataPoints > MAXVALIDDATAPTS )
		dataPoints = MAXVALIDDATAPTS;

	uint32_t newBufferSize = 2*dataPoints;	// --> always a multiple of 4!

	if( newBufferSize > MAXDATAPTS )
		newBufferSize = MAXDATAPTS;

	gDataBufferLength = newBufferSize;
	gDataLength = newBufferSize/2;
	gHalfDataLength = newBufferSize/4;

	if( dmaReset )
	{
		//reset DMA here and configure accordingly
		DAQ12_stop();
		DMA_setup_peri( 1, 1, &ADC1_2_COMMON->CDR, (uint32_t*)(MEMBASE12+0x4), 32, true, gDataBufferLength );
		//DMA_setup_peri( 2, 5, &ADC3_4_COMMON->CDR, (uint32_t*)(MEMBASE34+0x4), 32, true, gDataBufferLength );	//DMA2 channel 5
		DAQ12_start();
	}
	return gDataLength;
}
