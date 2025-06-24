#include "uefi.h"
#include "palloc.h"

#define PAGE_SIZE 0x1000

static long total_pages;
static uint64* top_of_page_stack = (uint64*) -1;


uint64 init_alloc(EFI_MEMORY_DESCRIPTOR* memory_map, uintn memory_map_size)
{
	int mem_desc_count = memory_map_size / sizeof(EFI_MEMORY_DESCRIPTOR);	
	
	uint64* page_header_p = (uint64*) 0;
	for(uint64 i = 0; i < mem_desc_count; i++)
	{
		if(memory_map[i].type != EfiConventionalMemory)
		{ continue; }
	
		uint64 base_address = (uint64) memory_map[i].physical_address;
		if(top_of_page_stack == (uint64*) -1) { top_of_page_stack = (uint64*) base_address; }
		uint64 pages_in_bytes = (memory_map[i].number_of_pages)*PAGE_SIZE;

		*(page_header_p) = base_address;
		page_header_p = (uint64*) base_address;

		while(page_header_p < (uint64*) (base_address+pages_in_bytes))
		{
			if(page_header_p == (uint64*) (base_address+pages_in_bytes-PAGE_SIZE))
			{
				*(j) = -1;
				break;
			}
			
			*(page_header_p) = (uint64) page_header_p+PAGE_SIZE;
			page_header_p = (uint64*) *(page_header_p);
		}
		
	}

	return 0; 
}
