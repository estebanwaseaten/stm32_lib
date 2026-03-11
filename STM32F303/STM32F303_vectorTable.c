#include <stdint.h>
#include "../STM32.h"
#include "STM32F303.h"
#include "main.h"

extern uint32_t _estack;									//defined in linker script
extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss;		//defined in linker script


void Reset_Handler( void );
void Default_Handler( void );


__attribute__((section(".isr_vector"), used ))
volatile uint32_t isr_vectors[NUM_VECTORS] =
{
	[0] = (uint32_t)&_estack,			//top of stack from linker script
	[1] = (uint32_t)Reset_Handler,
	[2 ... NUM_VECTORS-1] = (uint32_t)Default_Handler,	//shall we add SPI interrup handler? YES YES YES
	//[16 + SPI1_IRQ]	= (uint32_t)SPI1_IRQHandler,
};


//we need to initiialize stuff here!
void Reset_Handler( void )
{
	//__disable_irq();

	SCB->VTOR = (uint32_t)&isr_vectors;

	//copy data (ram only):
//	for( uint32_t *s=&_sidata, *d=&_sdata; d < &_edata; )	I think this is not necessary, maybe??
//	{
//		*d++ = *s++;	this would just copy .data to .data
//	}

	// zero bss: (seems to be important for the program to run under certain conditions)
	for( uint32_t *d=&_sbss; d < &_ebss; )
	{
		*d++ = 0;
	}
	// cheesecake does the following already:
	/*	set VTOR to 0x2000 0000 or whatever address is givem
	 *	set SP to isr_vectors[0]
	 *  set some DBG ret to isr_vectors[1]... not sure why isr_vectors[1] should be the reset handler...
	*/
	//SCB->VTOR = (uint32_t)&isr_vectors;
	//__asm__( "LDR  r0, =_estack\nMOV  sp, r0" );

	//__enable_irq();
	main();
}

void Default_Handler( void )
{
	main();
}

void setHandler_DMA( uint32_t dma, uint32_t channel, isr_t fn )
{
	if( dma == 1 )			//DMA1 ch 1: 11 (18 - 1 + 1)
	{
		isr_vectors[16 + DMA1_CH1_IRQ - 1 + channel] = ((uint32_t)fn) | 1u;
	}
	else if( dma == 2 )
	{
		isr_vectors[16 + DMA2_CH1_IRQ - 1 + channel] = ((uint32_t)fn) | 1u;
	}
}

void setHandler_ADC( uint32_t adcnum, isr_t fn )
{
	if( (adcnum == 1) || (adcnum == 2) )
	{
		isr_vectors[16 + ADC1_2_IRQ] = ((uint32_t)fn) | 1u;
	}
	else if( adcnum == 3 )
	{
		isr_vectors[16 + ADC3_IRQ] = ((uint32_t)fn) | 1u;
	}
	else if( adcnum == 4 )
	{
		isr_vectors[16 + ADC4_IRQ] = ((uint32_t)fn) | 1u;
	}
}


void setHandler_SysTick( isr_t fn )		//should be called every ms?
{
	isr_vectors[16 - 1] =  ((uint32_t)fn) | 1u; // cast + Thumb bit;
	//priority??
}

void setHandler_SPI1( isr_t fn )
{
	isr_vectors[16 + SPI1_IRQ] =  ((uint32_t)fn) | 1u; // cast + Thumb bit;
}

void setHandler_TIM2( isr_t fn )		// 32 bit
{
	isr_vectors[ 16 + TIM2_IRQ] = ((uint32_t)fn) | 1u;
}

void setHandler_TIM3( isr_t fn )		// 16 bit
{
	isr_vectors[ 16 + TIM3_IRQ] = ((uint32_t)fn) | 1u;
}

void setHandler_TIM4( isr_t fn )		// 16 bit
{
	isr_vectors[ 16 + TIM4_IRQ] = ((uint32_t)fn) | 1u;
}

//offsets differ between different STM32 series...
/*void setHandler( uint32_t offset, isr_t fn )
{
	isr_vectors[ 16 + offset] = ((uint32_t)fn) | 1u;
}*/
