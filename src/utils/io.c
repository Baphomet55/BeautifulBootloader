void outl(int data, int io_addr )
{
	__asm__ __volatile__("mov %ecx, %eax");
	__asm__ __volatile__("outl %eax, %edx");
}

int inl(int io_addr)
{
	__asm__ __volatile__("mov %ecx, %edx");
	__asm__ __volatile__("inl %edx, %eax");
}
