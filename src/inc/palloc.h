struct PAGE{
	struct PAGE* next_page;
	uint64 free[511];	
};

typedef struct PAGE PAGE;

uint64 init_alloc(EFI_MEMORY_DESCRIPTOR*, uintn);

void* alloc_page();

void free_page(void*);
