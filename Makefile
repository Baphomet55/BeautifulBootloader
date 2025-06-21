boot : boot.so
	@if [ $$DEBUG == "1" ]; then\
		objcopy -R .note.gnu.property -O pei-x86-64 --subsystem=10 boot.so main.efi;\
	else\
		objcopy -R .note.gnu.property -O pei-x86-64 --subsystem=10 boot.so bootx64.efi;\
	fi

boot.so : boot.o script.lds
	ld -Tscript.lds boot.o -o boot.so


boot.o : test.s
	as test.s -o boot.o

install : 
	@if ls main.efi ; then\
		sudo mount poop.iso /mnt/iso;\
		sudo rm /mnt/iso/EFI/BOOT/*;\
		sudo cp main.efi /mnt/iso/EFI/BOOT;\
		sudo cp shellx64.efi /mnt/iso/EFI/BOOT;\
		sudo mv /mnt/iso/EFI/BOOT/shellx64.efi /mnt/iso/EFI/BOOT/bootx64.efi;\
		sudo umount /mnt/iso;\
	else\
		sudo mount poop.iso /mnt/iso;\
		sudo rm /mnt/iso/EFI/BOOT/*;\
		sudo cp bootx64.efi /mnt/iso/EFI/BOOT;\
		sudo umount /mnt/iso;\
	fi
	

all :
	make
	make install


clean : 
	rm boot.so boot.o
	@if ls main.efi ; then\
		rm main.efi;\
	fi
	@if ls bootx64.efi ; then\
		rm bootx64.efi;\
	fi

