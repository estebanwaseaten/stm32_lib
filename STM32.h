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


enum gpio_pin_functions
{
    PIN_OUTPUT,
    PIN_INPUT
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
//per timer:
uint32_t TIMER2_getClockHz( void );
uint32_t TIMER2_getCount( void );
uint32_t TIMER2_getCountTo( void );
//uint32_t TIMER2_getScale( void );
void TIMER2_setARRHI( uint32_t arrHi );
void TIMER2_setARRLO( uint32_t arrLo );

void TIMER2_clear_interrupt( void );
void TIMER2_setup( uint32_t us_between );

//general
void TIMER_init( uint32_t tim, bool pllSrc );
void TIMER_enable( uint32_t tim, uint32_t divider, bool pllSrc );
void TIMER2_start();
void TIMER2_stop();
void TIMER2_disable();



//GPIO
void GPIO_init( void );
void GPIO_changeFunction( uint32_t pin, uint32_t function );
void GPIO_set( uint32_t pin );
void GPIO_unset( uint32_t pin );
int GPIO_get( uint32_t pin );

//ADC
void ADC_init( uint32_t ADCnum );
void ADC_setup( uint32_t ADCnum );
void ADC12_setup_dual( void );
uint32_t ADC12_getClockHz( void );
uint32_t ADC12_maximize_sampling_time( uint32_t timArr, uint32_t timClk );
void ADC_enable( uint32_t ADCnum, bool manual );
void ADC_arm( uint32_t ADCnum );
void ADC_disarm( uint32_t ADCnum );

int ADC_disable( uint32_t ADCnum );
uint16_t ADC_read_single( uint32_t ADCnum );

//SPI
int SPI_init( uint32_t SPInum );
void SPI_reset( void );
int SPI_enable( uint32_t SPInum, uint8_t bitsPerWord );
int SPI_disable( uint32_t SPInum );
int SPI_isBusy( void );

int SPI_enable_interrupt( uint32_t SPInum, int interruptType );

void SPI_test( void );
int SPI_receive( void );
int SPI_send( uint16_t data );

//DMA
void DMA_init( bool dma1, bool dma2 );
void DMA_setup_peri( uint32_t dma, uint32_t chan, volatile uint32_t *sourceAddr, uint32_t *destAddr, uint32_t bitSize, bool circ, uint32_t bufferLength );
void DMA_reset( uint32_t DMAnum, uint32_t dma_channel );

void DMA_enable_interrupt( uint32_t DMAnum, uint32_t dma_channel );
void DMA_enable( uint32_t dma_num, uint32_t dma_channel );
//void DMA_setup( void );

void DMA_clear_interrupts( uint32_t DMAnum );

//DAQ subroutines
void DAQ12_setup( void );
void DAQ12_pause( void );
void DAQ12_resume( void );
void DAQ12_start( void );
void DAQ12_stop( void );

void DAQ_currentFetchDone( void );
void DAQ_prepFetch( uint32_t channel );

void DAQ_setARR( uint32_t arr );


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
