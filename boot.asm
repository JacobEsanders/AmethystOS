; Multiboot header for GRUB
MBALIGN     equ 1 << 0
MEMINFO     equ 1 << 1
FLAGS       equ MBALIGN | MEMINFO
MAGIC       equ 0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KiB
stack_top:

section .text
global _start
_start:
    mov esp, stack_top    ; Set up stack
    
    ; Initialize processor state
    cli                   ; Disable interrupts
    cld                   ; Clear direction flag
    
    ; Call the kernel
    extern kmain
    call kmain
    
    ; If kernel returns, enter infinite loop
    cli
.hang:
    hlt
    jmp .hang
