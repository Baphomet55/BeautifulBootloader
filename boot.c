#include "uefi.h"

asm(
".global _start\n"
".data\n"
"dummy:\n"
".long 0\n"
".section .reloc\n"
"label1:\n"
".long dummy-label1\n"
".long 10\n"
".word 0\n"
".text\n"
"_start:\n"
"call main\n"
);

int main(void* handle, EFI_SYSTEM_TABLE* system_table)
{
	long a = system_table->con_out->OutputString(system_table->con_out, L"Satan");
	while(true) {};
	return 0;
}
