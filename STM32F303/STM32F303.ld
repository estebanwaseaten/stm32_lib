MEMORY
{
	SRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 64K
	CCMSRAM (xrw) : ORIGIN = 0x10000000, LENGTH = 16K
	/*FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K*/
}

//_estack = 0x20010000;		/*only 64kB normal SRAM, rest is ...*/
_estack = 0x10004000;

INCLUDE ../stm32_lib/STM32F303/sections.ld
