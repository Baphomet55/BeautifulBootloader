ENTRY(_start)
SECTIONS
{
	. = 0x3000;
	.text : { *(.text) }
	. = ALIGN(4096);
	.reloc : { *(.reloc) }
	. = ALIGN(4096);
	.data : { *(.data) }
	. = ALIGN(4096);
	.bss : { *(.bss) }
	. = ALIGN(4096);
	.note.gnu.property : { *(.note.gnu.property) }
}
