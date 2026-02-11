SECTIONS
{
	. = ORIGIN(SRAM);		/* put vector table at start of SRAM*/
	.isr_vector : ALIGN(4)
	{
		KEEP(*(.isr_vector))
	} > SRAM

	.text : ALIGN(4)
	{				/* .text is code*/
		_stext = .;
		*(.text)
		*(.text.*)
		 _etext = .;
	} > SRAM

	.data : ALIGN(4)
	{				/* .data is initialized data */
		 _sdata = .;
		*(.data)
		*(.data.*)
		_edata = .;
	} > SRAM

	.bss : ALIGN(4)
	{				/* .bss is uninitialized data */
		_sbss = .;
		*(.bss)
		*(.bss.*)
		_ebss = .;
	} > SRAM
}
