CC = gcc
AS = nasm
LD = ld
OBJCOPY = objcopy
GRUB_MKRESCUE = grub-mkrescue

BUILD_DIR = build
ISO_DIR = $(BUILD_DIR)/iso
KERNEL_DIR = kernel
INCLUDE_DIR = include

CFLAGS = -m32 -ffreestanding -nostdlib -fno-builtin -Wall -I$(INCLUDE_DIR)
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

KERNEL_ELF = $(BUILD_DIR)/kernel.elf
ISO_IMAGE = $(BUILD_DIR)/IsThisAnOS.iso

C_SOURCES = \
    $(KERNEL_DIR)/main.c \
    $(KERNEL_DIR)/graphics.c \
    $(KERNEL_DIR)/font.c \
    $(KERNEL_DIR)/string.c \
    $(KERNEL_DIR)/io.c

ASM_SOURCES = boot.asm

ASM_OBJECTS = $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
C_OBJECTS = $(patsubst $(KERNEL_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

all: $(KERNEL_ELF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(ISO_DIR)/boot/grub

$(BUILD_DIR)/boot.o: boot.asm | $(BUILD_DIR)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(KERNEL_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_ELF): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)
	@echo "✓ Kernel built: $(KERNEL_ELF)"

iso: $(KERNEL_ELF) | $(BUILD_DIR)
	cp $(KERNEL_ELF) $(ISO_DIR)/boot/
	cp grub/grub.cfg $(ISO_DIR)/boot/grub/
	$(GRUB_MKRESCUE) -o $(ISO_IMAGE) $(ISO_DIR) 2>/dev/null
	@echo "✓ ISO created: $(ISO_IMAGE)"

run: iso
	@echo "Running kernel with graphics support..."
	qemu-system-x86_64 -cdrom build/IsThisAnOS.iso -serial stdio -m 512M

run-text: $(KERNEL_ELF)
	qemu-system-x86_64 -cdrom build/IsThisAnOS.iso -serial stdio -m 512M -nographic

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all iso run run-text clean
