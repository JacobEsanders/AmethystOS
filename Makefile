CC      = gcc
CFLAGS  = -O2 -g -Wall -Wextra -I./include -ffreestanding -nostdlib -fno-builtin
LDFLAGS = -nostdlib

KERNEL_OBJS = boot/boot.o \
              boot/paging.o \
              boot/context.o \
              kernel/kernel.o \
              system/init.o \
              system/memory.o \
              system/paging.o \
              system/process.o \
              system/filesystem.o \
              system/fs_utils.o \
              system/file_io.o \
              system/package.o \
              device/video.o \
              device/keyboard.o \
              shell/shell.o \
              shell/shell_io.o \
              shell/fs_commands.o \
              shell/pkg_commands.o

C_OBJS = kernel/kernel.o \
         system/init.o \
         system/memory.o \
         system/paging.o \
         system/process.o \
         system/filesystem.o \
         system/fs_utils.o \
         system/file_io.o \
         system/package.o \
         device/video.o \
         device/keyboard.o \
         shell/shell.o \
         shell/shell_io.o \
         shell/fs_commands.o \
         shell/pkg_commands.o \
         lib/string.o

.PHONY: all clean test

all: amethystos.bin

amethystos.bin: $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $< -o $@

%.o: %.asm
	nasm -f elf32 $< -o $@

clean:
	rm -f $(KERNEL_OBJS) amethystos.bin

test: $(C_OBJS)
	@echo "C files compiled successfully"
