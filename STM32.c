#include "STM32.h"

void waitCycles( uint32_t cycles )
{
	for( uint32_t i = 0; i < cycles; i++ )
	{
		__asm("nop");
	}
}
void setWord( uint32_t addr, uint32_t word )
{
	volatile uint32_t *ptr = (uint32_t*)addr;
	*ptr = word;
}

uint32_t getWord( uint32_t addr )
{
	volatile uint32_t *ptr = (uint32_t*)addr;
	return *ptr;
}
