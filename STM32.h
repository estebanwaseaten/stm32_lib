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

void CLOCK_enable_sysTick( uint32_t clockSpeed );


uint32_t CLOCK_get_sysClk( void );
uint32_t CLOCK_get_pllClk( void );


//GPIO
void GPIO_init( void );
void GPIO_changeFunction( uint32_t pin, uint32_t function );
void GPIO_set( uint32_t pin );
void GPIO_unset( uint32_t pin );
int GPIO_get( uint32_t pin );

//ADC
void ADC_init( void );
int ADC_enable( uint32_t ADCnum );
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
void DMA_init( void );

//TIMERS
uint32_t TIMER_getcount( uint32_t tim );
void TIMER_clear_interrupt( uint32_t tim );
void TIMER_init( void );
void TIMER_enable( uint32_t tim, uint32_t divider, bool pllSrc );
void TIMER_start( uint32_t tim );
void TIMER_disable( uint32_t tim );


// for setting custom handlers (implement as needed)
typedef void (*isr_t)(void);
void setHandler_SysTick( isr_t fn );
void setHandler_SPI1( isr_t fn );
void setHandler_TIM2( isr_t fn );   //general purpose timer
void setHandler_TIM3( isr_t fn );   //general purpose timer
void setHandler_TIM4( isr_t fn );   //general purpose timer


#endif
