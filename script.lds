ENTRY(_start)
SECTIONS
{
	. = 0x0;
	.eh_frame : { *(.eh_frame) }
	. = ALIGN(4096);
	.text : { *(.text) }
	. = ALIGN(4096);
	.reloc : { *(.reloc) }
	. = ALIGN(4096);
	.data : 
	{ 
	  *(.rodata) 
	  *(.data*) 
	}
	. = ALIGN(4096);
	.bss : { *(.bss) }
	. = ALIGN(4096);
	.note.gnu.property : { *(.note.gnu.property) }
	. = ALIGN(4096);
	.comment : { *(.comment) }
}
