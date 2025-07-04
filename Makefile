TARGET_EXEC := bootx64.efi

BUILD_DIR := ./build
SRC_DIRS := ./src

SRCS := $(shell find $(SRC_DIRS) -name '*.c')

OBJS := $(patsubst ./src/%.c, ./build/%.o, $(SRCS))

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CC = gcc
CFLAGS = $(INC_FLAGS) -Og -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -mabi=ms
LINKER_SCRIPT := script.lds
OBJCOPY_FLAGS := -j .text -j .reloc -j .data -j .bss -O pei-x86-64 --subsystem 10

$(BUILD_DIR)/$(TARGET_EXEC): $(BUILD_DIR)/boot.so
	objcopy $(OBJCOPY_FLAGS) $(BUILD_DIR)/boot.so $(BUILD_DIR)/$(TARGET_EXEC)

$(BUILD_DIR)/boot.so: $(OBJS)
	ld -T$(LINKER_SCRIPT) $(OBJS) -o $(BUILD_DIR)/boot.so

$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.c 
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

install:
	sudo mount poop.iso /mnt/iso
	sudo cp $(BUILD_DIR)/$(TARGET_EXEC) /mnt/iso/EFI/BOOT
	sudo umount /mnt/iso

clean:
	rm -rf build
