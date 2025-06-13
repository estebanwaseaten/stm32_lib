MEMORY
{
	SRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 8K
	/*FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K*/
}

_estack = 0x20002000;

INCLUDE ../STM32_lib/sections.ld	#weird.. maybe because we are inside the build folder"?
