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

static EFI_SYSTEM_TABLE* ST;
static EFI_BOOT_SERVICES* BS;
static EFI_MEMORY_DESCRIPTOR* memory_map = 0;

int main(void* handle, EFI_SYSTEM_TABLE* system_table)
{
	ST = system_table;
	BS = ST->BootServices;
	ST->ConOut->OutputString(system_table->ConOut, L"Satan");
	
	uintn memory_map_size = 0;
	uintn map_key;
	uintn descriptor_size;
	uint32 descriptor_version;
	BS->GetMemoryMap(
		&memory_map_size,
		memory_map,
		&map_key,
		&descriptor_size,
		&descriptor_version
		
	);

	memory_map_size += 2*descriptor_size;
	long status = BS->AllocatePool(
		EfiLoaderData,
		memory_map_size,
		(void**) &memory_map
		 
	);
		
	BS->GetMemoryMap(
		&memory_map_size,
		memory_map,
		&map_key,
		&descriptor_size,
		&descriptor_version
		
	);

	BS->ExitBootServices(handle, map_key);
	ST->ConOut->OutputString(system_table->ConOut, L"Satan");
	while(true) {};
	return 0;
}
