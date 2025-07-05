#include "uefi.h"
#include "phys_mem_m.h"

static uint64 pdpt_index;
static void** vaddr_pool;


int init_vaddr_space(void** pt)
{

	void** vaddr_pdpt = (void**) alloc_page();

	pt[1] = (void*) ((uint64)vaddr_pdpt | 0x23);

	vaddr_pool = (void**) ((uint64)pt[1] & ~(0xfff));
	return  0;
}

uint64 alloc_vaddr_space()
{
	uint64 virt_addr = (uint64) 1 << 39;
	
	if(!((uint64) vaddr_pool[pdpt_index] & 1))
		vaddr_pool[pdpt_index] = (void*) ((uint64) alloc_page() | 0x23);

	virt_addr |= pdpt_index << 30;

	uint64* vaddr_space = (uint64*) ((uint64) vaddr_pool[pdpt_index] & ~(0xfff));

	int i = 0;
	while(vaddr_space[i] & 1) {i++;}	

	vaddr_space[i] = (uint64) alloc_page() | 0x23;

	virt_addr |= i << 21;

	return virt_addr;
}

void* alloc_virt_mem(uint64 address_space)
{
	if(!(address_space & (uint64) (1L << 39)))
		return (void*) -1;

	uint64 pml3_index = (address_space >> 29) & 0x3ff;
	void** pml2 = (void**) ((uint64) vaddr_pool[pml3_index] & ~(0xfff));
	
	uint64 pml2_index = (address_space >> 20) & 0x3ff;

	uint64* pml1 = (uint64*) ((uint64) pml2[pml2_index] & ~(0xfff));

	int i = 0;
	while(pml1[i] & 1) {i++;}
	
	pml1[i] = (uint64) alloc_page() | 0xe3;

	address_space |= i << 12;

	return (void*) address_space;
}
