#include "uefi.h"
#include "phys_mem_m.h"

#define PAGE_SIZE 0x1000

static long phys_addr_space_sz;
static PAGE* top_of_page_stack = (PAGE*) -1;

int get_msr()
{
	__asm__ __volatile__("mov $0xC0000080, %rcx\nrdmsr");
}


void** get_page_table()
{
	__asm__ __volatile__("mov %cr3, %rax");
}

int set_page_table(void** pt_address)
{
	__asm__ __volatile__("mov %rcx, %cr3");
}

uint64 init_page_table(EFI_MEMORY_DESCRIPTOR* mem_map)
{
	return 0;
}


uint64 init_alloc(EFI_MEMORY_DESCRIPTOR* memory_map, uintn memory_map_size)
{
	int mem_desc_count = memory_map_size / sizeof(EFI_MEMORY_DESCRIPTOR);	
	
	PAGE* page_index;
	for(uint64 i = 0; i < mem_desc_count; i++)
	{
		phys_addr_space_sz += memory_map[i].number_of_pages;
		if(memory_map[i].type != EfiConventionalMemory)
		{ continue; }

		PAGE* base_address = (PAGE*) memory_map[i].physical_address;	
		uint64 page_count_b = memory_map[i].number_of_pages;		

		if(top_of_page_stack == (PAGE*) -1) { top_of_page_stack = base_address; }
		if(page_index < top_of_page_stack) { page_index = base_address; }
		

		page_index->next_page = base_address;
		page_index = base_address;
		while(page_index < (base_address+page_count_b-1) )
		{
			page_index->next_page = (page_index+1);
			page_index = page_index->next_page;
		}

		page_index->next_page = (PAGE*) -1;
		
	}

	return 0; 
}

void* alloc_page()
{
	PAGE* page = top_of_page_stack;
	top_of_page_stack = top_of_page_stack->next_page;
	return (void*) page;
}

void free_page(void* page_address)
{
	PAGE* freed_page = (PAGE*) page_address;
	freed_page->next_page = top_of_page_stack;
	top_of_page_stack = freed_page;
}
