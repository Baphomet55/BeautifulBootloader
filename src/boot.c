asm(
".data\n"
"dummy:\n"
".long 0\n"
".section .reloc\n"
<<<<<<< Updated upstream
"mock_entry:\n"
".long dummy-mock_entry\n"
".long 10\n"
".word 0\n"
".text\n"
);

int main()
=======
"mock_rel:\n"
".long mock_rel-dummy\n"
".long 10\n"
".word 0\n"
);

int boot()
>>>>>>> Stashed changes
{
	while(true) {;}
	return 0;
}
