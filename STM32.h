//common header

#ifndef STM32_H
#define STM32_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>     //NULL

//SPI interrupt types (can or them together)
#define SPI_TXEI 4
#define SPI_RXNEI 2
#define SPI_ERRI 1

enum spi_bits
{
    SPI_8BITSPERWORD,
    SPI_16BITSPERWORD
};

enum sys_clocks
{
    SYSCLK_HSI,
    SYSCLK_HSE,
    SYSCLK_PLL
};

enum gpio_banks
{
    GPIO_BANK_A,
    GPIO_BANK_B,
    GPIO_BANK_C,
    GPIO_BANK_D,
    GPIO_BANK_E,
    GPIO_BANK_F,
    GPIO_BANK_G,
    GPIO_BANK_H
};


#define GPIO_MODE_INPUT     0x0
#define GPIO_MODE_OUTPUT    0x1
#define GPIO_MODE_ALT       0x2
#define GPIO_MODE_ANALOG    0x3

#define GPIO_OTYPE_PUSHPULL     0x0
#define GPIO_OTYPE_OPENDRAIN    0x1

#define GPIO_SPEED_LOW      0x0
#define GPIO_SPEED_MEDIUM   0x1
#define GPIO_SPEED_HIGH     0x3

#define GPIO_PULL_NONE  0x0
#define GPIO_PULL_UP    0x1
#define GPIO_PULL_DOWN  0x2

enum alternateFunctions
{
    GPIO_ALT0,
    GPIO_ALT1,
    GPIO_ALT2,
    GPIO_ALT3,
    GPIO_ALT4,
    GPIO_ALT5,
    GPIO_ALT6,
    GPIO_ALT7,
    GPIO_ALT8,
    GPIO_ALT9,
    GPIO_ALT10,
    GPIO_ALT11,
    GPIO_ALT12,
    GPIO_ALT13,
    GPIO_ALT14,
    GPIO_ALT15
};

//data acquisition (DAQ):
#define CH1 0x1
#define CH2 0x2
#define CH3 0x4
#define CH4 0x8

enum trigger_modes
{
	TRIG_SOFTWARE,
	TRIG_CH1,
	TRIG_CH2,
	TRIG_CH3,
	TRIG_CH4,
    TRIG_CH1_AUTO,
	TRIG_CH2_AUTO,
	TRIG_CH3_AUTO,
	TRIG_CH4_AUTO
};




#define SETBIT( reg, bit ) ((reg) |= (1U << (bit)))
#define SETBITS( reg, bits, shift ) ((reg) |= ((bits) << (shift)))
#define SETWRD( reg, word ) ((reg) = (word))

#define CLRBIT( reg, bit ) ((reg) &= ~(1U << (bit)))
#define CLRBITS( reg, bits, shift ) ((reg) &= ~((bits) << (shift)))
#define CLRWRD( reg ) ((reg) = 0x0)

#define CHKBIT( reg, bit ) ((reg) & (1U <<(bit)))



//MISC
uint32_t getWord( uint32_t addr );
void setWord( uint32_t addr, uint32_t word );
void waitCycles( uint32_t cycles );


void STMtest( void );


//RCC (clock)
void CLOCK_init( uint32_t sysClk );
void CLOCK_start_HSI( void );
void CLOCK_start_HSE( void );
void CLOCK_start_PLL( uint32_t pllSource );

uint32_t CLOCK_get_sysClk( void );
uint32_t CLOCK_get_pllClk( void );
uint32_t CLOCK_get_AHB( void );
uint32_t CLOCK_get_PCLK1( void );
uint32_t CLOCK_get_PCLK2( void );

//systick
void SYSTICK_enable( uint32_t clockSpeed );
uint32_t SYSTICK_get_ms();
uint32_t SYSTICK_get_us();
uint32_t SYSTICK_get_ticks_raw();


//TIMERS
void TIMER_start_clock( uint32_t tim, bool pllSrc );

//really only the inlines should be separate functions for TIM#
void TIMER2_setup( uint32_t arr, uint32_t prescaler  );
void TIMER2_interrupt( bool enable );
void TIMER2_interrupt_enable( void );
void TIMER2_interrupt_clear( void );
uint32_t TIMER2_getClockHz( void );
void TIMER2_start( void );
void TIMER2_stop( void );
void TIMER2_resume( void );
void TIMER2_restart( void );
void TIMER2_disable( void );

void TIMER3_setup( uint32_t arr, uint32_t prescaler );
void TIMER3_enable( void );
void TIMER3_disable( void );
void TIMER3_start( void );
void TIMER3_stop( void );
void TIMER3_interrupt_enable( void );
void TIMER3_interrupt_clear( void );
uint32_t TIMER3_getCountTo( void );
uint32_t TIMER3_getPrescaler( void );
uint32_t TIMER3_getClockHz( void );

// DAC1
void DAC_start_clock( void );
void DAC_enable( uint32_t channel );
void DAC_set( uint32_t channel, uint16_t level );
void DAC_disable( uint32_t channel );

//COMP
void COMP_start_clock( void );
void COMP_select_output( uint32_t num, uint32_t out );
void COMP_select_input( uint32_t num, uint32_t in );


//GPIO
void GPIO_start_clock( void );
void GPIO_changeFunction( uint32_t bank, uint32_t pin, uint32_t function );
void GPIO_changeOutputType( uint32_t bank, uint32_t pin, uint32_t type );
void GPIO_changeOutputSpeed( uint32_t bank, uint32_t pin, uint32_t speed );
void GPIO_changePull( uint32_t bank, uint32_t pin, uint32_t pull );
void GPIO_altFunc( uint32_t bank, uint32_t pin, uint32_t function );

void GPIO_set( uint32_t pin );
void GPIO_unset( uint32_t pin );
uint32_t GPIO_get( uint32_t pin );


void ADC_debug(void);
//ADC
void ADC_start_clock( uint32_t ADCnum );
void ADC_init( uint32_t ADCnum );

void ADC_setup( uint32_t ADCnum );
void ADC12_setup_dual( void );
uint32_t ADC12_getClockHz( void );
uint32_t ADC12_maximize_sampling_time( uint32_t timArr, uint32_t prescaler, uint32_t timClk );
void ADC_enable( uint32_t ADCnum, bool manual );
void ADC_arm( uint32_t ADCnum );
void ADC_disarm( uint32_t ADCnum );

int ADC_disable( uint32_t ADCnum );
uint16_t ADC_read_single( uint32_t ADCnum );

void ADC_enable_watchdog( uint32_t ADCnum, uint16_t level );
void ADC_disable_watchdogs();

void ADC1_watchdog_clear_and_disarm( void );
void ADC1_watchdog_arm( void );

//SPI
int SPI_start_clock( uint32_t SPInum );
void SPI_reset( void );
int SPI_enable( uint32_t SPInum, uint8_t bitsPerWord );
int SPI_disable( uint32_t SPInum );
int SPI_isBusy( void );

int SPI_enable_interrupt( uint32_t SPInum, int interruptType );

void SPI_test( void );
int SPI_receive( void );
int SPI_send( uint16_t data );


//DMA
void DMA_start_clock( bool dma1, bool dma2 );

void DMA_setup_peri( uint32_t dma, uint32_t chan, volatile uint32_t *sourceAddr, uint32_t *destAddr, uint32_t bitSize, bool circ, uint32_t bufferLength );
void DMA_reset( uint32_t DMAnum, uint32_t dma_channel );

void DMA_enable_interrupt( uint32_t DMAnum, uint32_t dma_channel );
void DMA_enable( uint32_t dma_num, uint32_t dma_channel );
//void DMA_setup( void );

void DMA_clear_interrupts( uint32_t DMAnum );

uint16_t DMA_get_pos( uint32_t  DMAnum, uint32_t dma_channel );


/********* DAQ subroutines *********/
void DAQ_init( void );

//config
void DAQ_config_timebase( uint8_t timebase );
uint32_t DAQ_config_dataBuffer( uint32_t dataPoints );
void DAQ_config_trigger_mode( uint16_t newMode );
void DAQ_config_trigger_level( uint16_t newLevel );
void DAQ_config_trigger_pos( uint16_t newPos );
void DAQ_config_trigger_risingEdge( bool rising );

void DAQ_config_update( void );    //stops, writes all the values and potentially starts again???

uint8_t DAQ_config_timebase_get( void );
uint16_t DAQ_config_trigger_mode_get( void );
uint16_t DAQ_config_trigger_level_get( void );
uint16_t DAQ_config_trigger_pos_get( void );
bool DAQ_config_trigger_risingEdge_get( void );



//DAQ flow control
void DAQ12_pause( void );
void DAQ12_resume( void );
void DAQ12_start( void );
void DAQ12_stop( void );

// fetching data
void DAQ_prepFetch( uint32_t channel );
void DAQ_currentFetchDone( void );


//interrupt stuff
void Interrupt_Enable( uint32_t irq );

// for setting custom handlers (implement as needed)
typedef void (*isr_t)(void);
void setHandler_SysTick( isr_t fn );
void setHandler_SPI1( isr_t fn );
void setHandler_TIM2( isr_t fn );   //general purpose timer
void setHandler_TIM3( isr_t fn );   //general purpose timer
void setHandler_TIM4( isr_t fn );   //general purpose timer
void setHandler_DMA( uint32_t dma, uint32_t channel, isr_t fn );
void setHandler_ADC( uint32_t adcnum, isr_t fn );


#endif
