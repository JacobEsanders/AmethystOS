; Paging-related assembly functions
global enable_paging
global load_page_directory

section .text

enable_paging:
    push ebp
    mov ebp, esp
    
    ; Load CR3 with page directory address
    mov eax, [ebp + 8]
    mov cr3, eax
    
    ; Enable paging (set PG bit)
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    
    mov esp, ebp
    pop ebp
    ret

load_page_directory:
    push ebp
    mov ebp, esp
    
    ; Load CR3 with page directory address
    mov eax, [ebp + 8]
    mov cr3, eax
    
    mov esp, ebp
    pop ebp
    ret
