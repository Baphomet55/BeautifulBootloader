#include "uefi.h"
#include "palloc.h"

#define PAGE_SIZE 0x1000

typedef struct{
	uint64 next_page;
	uint64 free[511];	
} PAGE;

static long total_pages;
static PAGE* top_of_page_stack = (PAGE*) -1;


uint64 init_alloc(EFI_MEMORY_DESCRIPTOR* memory_map, uintn memory_map_size)
{
	int mem_desc_count = memory_map_size / sizeof(EFI_MEMORY_DESCRIPTOR);	
	
	uint64* page_header_p = (uint64*) 0;
	PAGE* page_index;
	for(uint64 i = 0; i < mem_desc_count; i++)
	{
		if(memory_map[i].type != EfiConventionalMemory)
		{ continue; }

		PAGE* base_address = (PAGE*) memory_map[i].physical_address;	
		uint64 page_count_b = memory_map[i].number_of_pages;		

		if(top_of_page_stack == (PAGE*) -1) { top_of_page_stack = base_address; }
		if(page_index < top_of_page_stack) { page_index = base_address; }
		

		page_index->next_page = (uint64) base_address;
		page_index = base_address;
		while(page_index < (base_address+page_count_b-1) )
		{
			page_index->next_page = (uint64) (page_index+1);
			page_index = (PAGE*) page_index->next_page;
		}

		page_index->next_page = -1;
		
	}

	return (uint64) page_index; 
}
