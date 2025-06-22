boot : boot.so
	@if [ $$DEBUG == "1" ]; then\
		objcopy -j .text -j .data -j .bss -j .reloc -O pei-x86-64 --subsystem=10 boot.so main.efi;\
	else\
		objcopy -j .text -j .data -j .bss -j .reloc -O pei-x86-64 --subsystem=10 boot.so bootx64.efi;\
	fi

boot.so : boot.o script.lds
	ld -Tscript.lds boot.o -o boot.so


boot.o : test.s
	gcc -I./ -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -mabi=ms -c boot.c -o boot.o
	as init.s -o init.o

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
	rm boot.so boot.o init.o
	@if ls main.efi ; then\
		rm main.efi;\
	fi
	@if ls bootx64.efi ; then\
		rm bootx64.efi;\
	fi

