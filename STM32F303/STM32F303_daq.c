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
//bool     gRequestArmWatchdog;   //not needed soon ()
bool gScheduledTIM2restart;


//external...
//settings that can be changed via SPI and define the DAQ
//uint32_t g_settings_datapoints;			// fixed at 1024 (max)
uint32_t g_settings_adc_time;			// 000 = 1.5 cycles to 111 = 601,5 cycles per ADC - must be set in ADC SMPR1 register
uint32_t g_settings_acquisition_speed;	// how many datapoints per second (defines timer settings)
uint32_t g_settings_enabledChannels;


// internal config values
bool gDAQ_is_running;

// save?
uint16_t gPrescaler;
uint32_t gArrTIM2;
uint16_t gArrTIM3;

// these values are changed by call to DAQ_config_DataBuffer()
uint32_t gDataBufferLength;
uint32_t gDataLength;			// --> circular buffer 200
uint32_t gHalfDataLength;

uint32_t gTriggerMode;		// 0 --> software, 1, 2, 3 or 4 hardware
uint32_t gTriggerLevel;
uint32_t gTriggerPos;
uint32_t gTriggerPosRemaining;
bool gTriggerRisingEdge;



volatile uint32_t gDataReady;	          //binary encoding 0b1111 for all four channels, 0b0001 for channel 1

//for fetching:
volatile uint32_t gCurrentFetchChannel;
volatile uint32_t gDataStart;		         //channel 1 & 2 --> 0x20000000, ...
volatile uint32_t gDataBitShift;		  //channel1: 0 channel2: 16
volatile uint32_t gDataTransferSize;	//number of datapoints
volatile uint32_t gDataIndex; 			//points to current mem location during fast data transfer
volatile uint32_t gDataCounter;			//start with 0 and count up to gDataTransferSize
volatile uint32_t gDatapointsAcquired;


void myDMA1_handler( void );
void myDMA2_handler( void );
void myADC_watchdog_handler( void );    //shall be removed

void myTIM2_handler( void );        // stops or pauses TIM2 --> ends DAQ in hardware triggered scheme

void myEXTI_handler( void );        // called after trigger event --> immediately starts TIM3

//		correct order
// 1. DMA config (NOT ENABLE!)
// 2. ADC config mit DMAEN  --> DMA will be used to store results
// 3. DMA enable            --> ADC + DMA setup is done.

// 4. TIM3 configuration    --> TIM3 will trigger ADC data acquisition
//    TIM2 will count after hardware trigger and its update interrupt will determine end of DAQ in this case

//for hardware trigger:
// 5. DAC to set reference level for comparator
// 6. COMP connected to TIM2
// 7. TIM2 to count down ADC conversions after trigger has fired --> g_settings_triggerPosRemaining to zero --> interrupt to stop ADC and wait for readout of data or restart
// clock can only be the main clock, because otherwise we cannot trigger this...

//this sets up the DAQ system
//clocks need to be enabled first
void DAQ_init( void )
{
    gDAQ_is_running = false;
    gDataReady = 0;

	//default starting values:
	//gHardwareTrigger = false;
    //gRequestArmWatchdog = false;
	gTriggerHappened = false;
	gLastTriggerEvent = 0;				// record DMA pos of last trigger event. Check if there is enough datapoints after 1/2- and 1-DMA interrupt
	gCurrentFetchChannel = 0;
    gScheduledTIM2restart = true;

    g_settings_enabledChannels = 0x1;

	/* enable interrup handlers for
 	 *		- DMA transfers (to check if we acquired enough datapoints after trigger event or in general)
	 *		- ADC watchdog (to)
	 */
	setHandler_DMA( 1, 1, myDMA1_handler );
	//setHandler_DMA( 2, ..., myDMA2_handler );

    //setHandler_ADC( 1, myADC_watchdog_handler );	// handler for ADC1
    //setHandler_ADC( 2, myADC_watchdog_handler );	// handler for ADC2
    //setHandler_ADC( 3, myADC_watchdog_handler );	// handler for ADC3
    //setHandler_ADC( 4, myADC_watchdog_handler );	// handler for ADC4

    setHandler_TIM2( myTIM2_handler );	//this timer will be counting "after trigger event" --> the handler will pause/stop acquisition

    //setHandler_EXTI( );

    //start all the peripheral clocks:
	DMA_start_clock( true, true );             //init both DMA channels

    ADC_start_clock( 1 );
    ADC_start_clock( 2 );

    DAC_start_clock();     // DAC1_CH1/PA4 can be used for all the comparators

    COMP_start_clock();

    TIMER_start_clock( 2, true );		//init timer 2 with PLL
    TIMER_start_clock( 3, true );		//init timer 3 with PLL

    // switch on voltage regulators:
    ADC_init( 1 );
    ADC_init( 2 );
    //ADC_init( 3 );
    //ADC_init( 4 );


    //DMA setup:
	DMA_setup_peri( 1, 1, &ADC1_2_COMMON->CDR, (uint32_t*)0x20000004, 32, true, gDataBufferLength );

    //ADC setup
	ADC12_setup_dual();

    //DAC & COMP setup for HW triggering:
    DAC_enable( 2 );
    DAC_set( 2, 2048 );

    COMP_select_output( 5, 0x6 );   //0b0110    - Timer 2 input capture 1
    COMP_select_input( 5, 0x5 );    //0b101     - PA5 or DAC2 output if enabled (PA4 is occupied)
                                    //we have to use DAC2!!
    //TIMER setup

    //default config:
    DAQ_config_dataBuffer( 500 );	//no dma reset here
    DAQ_config_timebase( 0x11 );    // 0b00010001 exp and factor = 1
	DAQ_config_trigger_mode( TRIG_SOFTWARE );
	DAQ_config_trigger_level( 0x4FF );
	DAQ_config_trigger_pos( 2000 );

    DAQ_config_update();

    //TIMER2_setup( gARR );	         //1 us between each acquisition --> 8000 acquisitions in 8ms
    //TIMER3_setup( gDataLength );     // ARR value can always be gDataLength (we have saved 2 x gDataLength, so we can freely pick)...
    // this means the trigger always happened gDataLength points before the TIM3 interrupt fires

    // AFTER configuring ADC we can enable DMA otherwise there is a problem
    DMA_enable_interrupt( 1, 1 );		//one interrupt per ADC dual pair
	DMA_enable( 1, 1 );
	//	DMA_enable( 2, 1 );	not needed in dual mode

    //start DAQ (enables timer 2)
//	DAQ12_start();
}

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

        if( gTriggerMode != TRIG_SOFTWARE )
        {
            //hardware triggering:


        }
	}

	if( CHKBIT(DMA1->ISR, 1 ) )		//Transfer complete (TC) flag for channel 1
	{
		setWord( 0x2000903C, getWord( 0x2000903C ) + 1 );	//debug
		//depends on mode if data is ready or not?
        if( gTriggerMode == TRIG_SOFTWARE )     //always ready after full DMA --> set gDataReady
		{
			gDataReady = g_settings_enabledChannels;
		}
        else
        {
            //hardware triggering:
            //gDataReady is set by TIM2 handler

        }

        if( gScheduledTIM2restart )
        {
            TIMER2_restart();   //setup has to be done before
        }

	}
	DMA_clear_interrupts( 1 );
}

void myDMA2_handler( void )		//needed? later maybe
{
	//setWord( 0x20009038, getWord( 0x20009038 ) + 1 );
}


// hardware trigger event happened DAC+COMP1-->EXTI--> hander: starts TIM3
void myEXTI_handler( void ) //NOT NEEDED CURRENTLY
{
    //TIM3 should be configured
    //1. start TIM3

    //2. delete EXTI flag?      EXTI->PR = EXTI_PR_PR_Pos; // Flag löschen
}

// STOPS data acquisition in hardware trigger mode:
// shall be triggered after a hardware trigger event + x ADC cycles.
// this timer will be counting "after trigger event" --> the handler will pause/stop acquisition
void myTIM2_handler( void )
{
    setWord( 0x20009064, getWord(0x20009064) + 1 );
    // stop TIM2
    TIMER3_stop();      //Immediately stop TIM3 (CEN=0) or clear EXTEN=0.

    TIMER2_interrupt_clear();       //needs to be a fast function
    TIMER2_disable();         // so we dont re-trigger...
    //reset TIMER3 counter...

    gDataReady = g_settings_enabledChannels;
    setWord( 0x20009004, getWord( 0x20009004 ) + 1 );
}



//thrown at hardware trigger event
/*void myADC_watchdog_handler( void )     //not triggering currently
{
    // 1. save current DMA position
    gLastTriggerEvent = DMA_get_pos( 1, 1 );

    // 2. disable watchdog
    ADC1_watchdog_clear_and_disarm();  //until the data is acquired: no more triggers:

    //set trriggered flag:
    gTriggerHappened = true;

    //debug:
    setWord( 0x20009024, gLastTriggerEvent );
}*/


/********* DAQ data fetch *********/
void DAQ_currentFetchDone( void )
{
	//invalidate data for this channel	//gCurrentFetchChannel
	gDataCounter = 0;
	gDataIndex = 0;
	CLRBIT( gDataReady, gCurrentFetchChannel - 1 );

    //for hardware triggering we should probably reset the DMA and disarm the TIM3 (until enough data is acquired)?
    //does this depend if there are more than 1 channels to fetch?
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

	gDataTransferSize = gDataLength;
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





/* Functions that start/stop/pause the DAQ
 */


//should this always also reset watchdogs etc...

// ADC1->DMA1CH1, (ADC2->DMA2CH1), ADC3->DMA2CH5, (ADC4->DMA2CH2) dual mode only uses the ADC-master DMA channels
void DAQ12_start( void )
{
    gDAQ_is_running = true;

    TIMER3_stop();          //important, otherwise ADC gets messed up during DMA_reset.

    //reload DMA:
    DMA_reset( 1, 1 );     //reset DMA (especially the counter CNDTR)
	//DMA_reset( 2, 1 ); DMA2,1 is not used in DUAL mode

    SETBIT( ADC[1]->ISR, 4 );      // clear OVR flag just in case
		SETBIT( ADC[2]->ISR, 4 );      // clear OVR flag just in case
    SETBIT( ADC[1]->CR, 2 );       // ADC armed --> TRGO starts conversion
		SETBIT( ADC[2]->CR, 2 );       // ADC armed --> TRGO starts conversion

    TIMER3_start();		//works - always start timer LAST!!! (so all the ADCs start in sync)
}

//after trigger event when the measuremen is done. stop timer:
void DAQ12_pause( void )
{
    TIMER3_stop();
}

//after data is read and resume command is issued. restart timer:
void DAQ12_resume( void )
{
    setWord( 0x20009060, getWord(0x20009060) + 1 );
    TIMER3_start(); //
}

void DAQ12_stop( void )
{
    TIMER3_stop();       //CLRBIT( TIM2->CR1, 0 );

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

    gDAQ_is_running = false;

    //TCIF löschen
    //SETWRD( DMA1->IFCR, 0xFFFFFFFF );
}



/* Functions that change DAQ settings
 */

void DAQ_config_trigger_mode( uint16_t newMode )
{
	gTriggerMode = newMode;
}

void DAQ_config_trigger_level( uint16_t newLevel )
{
    gTriggerLevel = newLevel;
}

void DAQ_config_trigger_pos( uint16_t newPos )
{
    if( newPos > gDataLength )
    {
        newPos = gDataLength;
    }
    gTriggerPos = newPos;
    gTriggerPosRemaining = gDataLength - gTriggerPos - 1;   //-1?
}

void DAQ_config_trigger_risingEdge( bool rising )
{
    gTriggerRisingEdge = rising;
}

uint32_t myPowerOfTen( uint32_t exp )
{
    if( exp == 0 )
        return 1;

    uint32_t result = 1;
    for( int i = 0; i < exp; i++ )
    {
        result *= 10;
    }
    return result;
}

// first 4 bits of timebase indicate factor,
// second 4 bits of timebase indicate exponent: factor * 10^exponent
// factor: 0,1 ... 1 (really the minimum is 0,2us here)
// exponent 0 ... 15 (really the maximum should be in the lower ms range)
// MAXVALIDDATAPTS is 4000 --> 2,5ms would result in a 10s measurement time for the whole trace
void DAQ_config_timebase( uint8_t timebase )
{
    // 0beeeeffff
    uint8_t factor = timebase & 0xF;
    uint8_t exponent = (timebase >> 4) & 0xF;

    if( factor < 1 )
    { factor = 1; }             // --> 0.1 * 10^exponent
    else if( factor > 10 )
    { factor = 10; }            // --> 1 * 10^exponent

    if( exponent > 8 )
    { exponent = 8; }   //prevent enormous measurement times

    uint32_t clk = TIMER2_getClockHz()/1000000;
    // tb < 1 ms --> prescaler = 0
    // tb >= 1ms --> prescaler = 72
    // TIM2 --> counts remaining datapoints after hardware trigger
    // TIM3 --> counts between ADC cycles

    //setWord( 0x20009030, timebase );

    uint32_t arr_tim2;
    uint32_t arr_tim3;
    uint32_t prescaler = 0;
    if( exponent == 0 ) //--> < 1ms
    {
        //calc arr and set
        prescaler = 0;
        arr_tim3 = (clk*factor/10) - 1; // counts to zero?
        arr_tim2 = ((arr_tim3+1) * gDataLength) - 1;
    // /    TODO!!!
    }
    else if( exponent == 1 )
    {
        prescaler = 0;
        arr_tim3 = (clk*factor) - 1; // counts to zero?
        arr_tim2 = ((arr_tim3+1) * gDataLength) - 1;
    }
    else
    {
        //set prescalers to clk-1
        prescaler = clk - 1;
        arr_tim3 = (factor * myPowerOfTen( exponent - 1) ) - 1;   //counts to arr+1
        arr_tim2 = ((arr_tim3+1) * gDataLength) - 1;    //counts to... ?
    }

    gPrescaler = prescaler;
    gArrTIM2 = arr_tim2;
    gArrTIM3 = arr_tim3;
    //setWord( 0x20009034, arr_tim3 );
    //setWord( 0x20009038, arr_tim2 );
    //setWord( 0x2000903C, prescaler );
}



uint32_t DAQ_config_dataBuffer( uint32_t dataPoints )		//half the data size --> guarantees multiple of 4
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

	return gDataLength;
}

// used: gArrTIM2, gArrTIM3, gPrescaler, gDataBufferLength
//		correct order
// 4. TIM3 configuration    --> TIM3 will trigger ADC data acquisition
//    TIM2 will count after hardware trigger and its update interrupt will determine end of DAQ in this case

//for hardware trigger:
// 5. DAC to set reference level for comparator
// 6. COMP connected to TIM2
// 7. TIM2 to count down ADC conversions after trigger has fired --> g_settings_triggerPosRemaining to zero --> interrupt to stop ADC and wait for readout of data or restart
// clock can only be the main clock, because otherwise we cannot trigger this...
void DAQ_config_update( void )
{
    setWord( 0x20009050, getWord(0x20009050) + 1 );  // Aufruf-Zähler
    setWord( 0x20009054, gArrTIM3 );                  // welcher ARR

    bool wasRunning = gDAQ_is_running;

    if( wasRunning )
    {
        DAQ12_stop();
    }

    gDataReady = 0;

    //config trigger mode, level etc
    if( gTriggerMode > 0 ) //hardware trigger
	{
        //fill data buffer once, then enable TIM2?
        // DMA reset & TIMER3_setup is done below always --> TIMER2_restart(); after DMA full handler
        gScheduledTIM2restart = true;
    }
    else    //software trigger
    {
        //disable TIM2, run TIM3
        TIMER2_stop();  //should this happen after imer setup?
        CLRBIT( TIM2->DIER, 0 );         // interrupt deaktivieren
        CLRBIT( TIM2->SR, 0 );           // pending flag clearen
        uint32_t bank = (TIM2_IRQ >> 5);
        SETBIT( NVIC_ICPR->BANK[bank], TIM2_IRQ - bank*32 );  // NVIC pending clearen
    }
    //DAC & COMP




    //config timers
    // 1. hardware trigger timer:
    TIMER2_setup( gArrTIM2, gPrescaler );
    TIMER2_interrupt_enable();
    // 2. general DAQ timer:
    TIMER3_setup( gArrTIM3, gPrescaler );

    //ADC:
    uint32_t clk = TIMER3_getClockHz();
    ADC12_maximize_sampling_time( gArrTIM3, gPrescaler, clk );


    //update DMA for channels 1 & 2
    DMA_setup_peri( 1, 1, &ADC1_2_COMMON->CDR, (uint32_t*)(MEMBASE12+0x4), 32, true, gDataBufferLength );

    if( wasRunning )
    {
        DAQ12_start();
    }
}

uint16_t DAQ_config_trigger_mode_get( void )
{
    return gTriggerMode;
}

uint16_t DAQ_config_trigger_level_get( void )
{
    return gTriggerLevel;
}

uint16_t DAQ_config_trigger_pos_get( void )
{
    return gTriggerPos;
}

bool DAQ_config_trigger_risingEdge_get( void )
{
    return gTriggerRisingEdge;
}
