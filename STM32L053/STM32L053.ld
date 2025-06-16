MEMORY
{
	SRAM (xrw) : ORIGIN = 0x20000000, LENGTH = 8K
	/*FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 64K*/
}

_estack = 0x20001800;	# I would like to use 0x20001800 - 2x20002000 for debugging

INCLUDE ../stm32_lib/sections.ld	#weird.. maybe because we are inside the build folder"?
