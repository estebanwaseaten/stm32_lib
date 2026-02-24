SECTIONS
{
	. = ORIGIN(CCMSRAM);		/* put vector table at start of SRAM*/
	.isr_vector : ALIGN(512)
	{
		KEEP(*(.isr_vector))
	} > CCMSRAM

	.text :
	{				/* .text is code */
		 . = ALIGN(4);
		_stext = .;
		*(.text)
		*(.text.*)
		*(.rodata)
 	    *(.rodata*)
		 _etext = .;
		. = ALIGN(4);
	} > CCMSRAM


	.data :
	{				/* .data is initialized data */
		. = ALIGN(4);
		 _sdata = .;
		*(.data)
		*(.data.*)
		. = ALIGN(4);
		_edata = .;
	} > CCMSRAM
	PROVIDE(_sidata = LOADADDR(.data));


	.bss (NOLOAD) :
	{				/* .bss is uninitialized data */
		. = ALIGN(4);
		_sbss = .;
		*(.bss)
		*(.bss.*)
		. = ALIGN(4);
		_ebss = .;
	} > CCMSRAM
}
