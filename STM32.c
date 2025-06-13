#include "STM32.h"


void setWord( uint32_t addr, uint32_t word )
{
	volatile uint32_t *ptr = (uint32_t*)addr;
	*ptr = word;
}
