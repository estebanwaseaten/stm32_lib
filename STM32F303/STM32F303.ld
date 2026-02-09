MEMORY
{
	SRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 64K
	/*FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K*/
}

_estack = 0x20010000;		/*only 64kB normal SRAM, rest is ...*/

INCLUDE ../stm32_lib/sections.ld
