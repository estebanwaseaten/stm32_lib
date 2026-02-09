#include <stdint.h>
#include "../STM32.h"
#include "STM32F303.h"
#include "main.h"

extern uint32_t _estack;		//defined in linker script

void Reset_Handler( void );
void Default_Handler( void );
void SPI1_Handler( void );



__attribute__((section(".isr_vector"), used ))
uint32_t isr_vectors[NUM_VECTORS] =
{
	[0] = (uint32_t)&_estack,			//top of stack from linker script: 0x20010000
	[1] = (uint32_t)Reset_Handler,
	[2 ... NUM_VECTORS-1] = (uint32_t)Default_Handler,	//shall we add SPI interrup handler? YES YES YES
	[16 + SPI1_IRQ]	= (uint32_t)SPI1_Handler,
};




void Reset_Handler( void )
{
	main();
}

void Default_Handler( void )
{
	main();
}

void SPI1_Handler( void )
{
    SPI_interrupt( 1 );
}
