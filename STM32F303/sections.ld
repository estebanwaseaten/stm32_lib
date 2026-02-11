SECTIONS
{
	. = ORIGIN(CCMSRAM);		/* put vector table at start of SRAM*/
	.isr_vector : ALIGN(4)
	{
		KEEP(*(.isr_vector))
	} > CCMSRAM

	.text : ALIGN(4)
	{				/* .text is code*/
		_stext = .;
		*(.text)
		*(.text.*)
		 _etext = .;
	} > CCMSRAM

	.data : ALIGN(4)
	{				/* .data is initialized data */
		 _sdata = .;
		*(.data)
		*(.data.*)
		_edata = .;
	} > CCMSRAM

	.bss : ALIGN(4)
	{				/* .bss is uninitialized data */
		_sbss = .;
		*(.bss)
		*(.bss.*)
		_ebss = .;
	} > CCMSRAM
}
