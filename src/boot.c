#include "uefi.h"

asm(
".data\n"
"dummy:\n"
".long 0\n"
".section .reloc\n"
"mock_entry:\n"
".long dummy-mock_entry\n"
".long 10\n"
".word 0\n"
".text\n"
);

uint64 debug(uint64 val)
{
	return val;
}

int main(void* handle, EFI_SYSTEM_TABLE* system_table)
{
	uintn memory_map_size = 0;
	EFI_MEMORY_DESCRIPTOR* memory_map;
	uintn map_key = 0;
	uintn descriptor_size = 0;
	uint32 descriptor_version = 0;

	system_table->BootServices->GetMemoryMap(
		&memory_map_size,
		memory_map,
		&map_key,
		&descriptor_size,
		&descriptor_version
	);

	memory_map_size += 2*descriptor_size;

	system_table->BootServices->AllocatePool(
		EfiLoaderData,
		memory_map_size,
		(void**) &memory_map
	);
	debug((uint64) memory_map);
	while(true) {;}
	//system_table->BootServices->GetMemoryMap();
	//system_table->BootServices->ExitBootServices();
	
	return 0;
}
