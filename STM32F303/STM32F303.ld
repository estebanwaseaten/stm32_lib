MEMORY
{
	SRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 80K
	/*FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K*/
}

_estack = 0x20010000;		/*really its 20014000, leave rest for debug ... does not work for 0x12000 ... need to figure out why*/

INCLUDE ../stm32_lib/sections.ld
