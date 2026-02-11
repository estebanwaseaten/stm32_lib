#include <stdint.h>
#include "../STM32.h"
#include "STM32F303.h"
#include "main.h"

extern uint32_t _estack;		//defined in linker script

void Reset_Handler( void );
void Default_Handler( void );


__attribute__((section(".isr_vector"), used ))
uint32_t isr_vectors[NUM_VECTORS] =
{
	[0] = (uint32_t)&_estack,			//top of stack from linker script: 0x20010000
	[1] = (uint32_t)Reset_Handler,
	[2 ... NUM_VECTORS-1] = (uint32_t)Default_Handler,	//shall we add SPI interrup handler? YES YES YES
	//[16 + SPI1_IRQ]	= (uint32_t)SPI1_IRQHandler,
};

void Reset_Handler( void )
{
	setWord( 0x20009020, 0xAB );		//does not seem to work... not sure why I cannot reset from SRAM...
	// cheesecake does the following:
	/*	set VTOR to 0x2000 0000
	 *	set SP to isr_vectors[0]
	 *  set some DBG ret to isr_vectors[1]... not sure why
	*/
	//SCB->VTOR = (uint32_t)&isr_vectors;
	//__asm__( "LDR  r0, =_estack\nMOV  sp, r0" );
	main();
}

void Default_Handler( void )
{
	main();
}

void setHandler_SPI1( isr_t fn )
{
	isr_vectors[16 + SPI1_IRQ] =  ((uint32_t)fn) | 1u; // cast + Thumb bit;
}
