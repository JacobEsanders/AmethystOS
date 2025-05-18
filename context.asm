; Context switching functionality
global context_switch

section .text

context_switch:
    ; Save current context if it exists
    ; old_context is in [esp + 4]
    ; new_context is in [esp + 8]
    
    mov eax, [esp + 4]    ; Get old_context
    test eax, eax         ; Check if NULL
    jz load_new          ; If NULL, just load new context
    
    ; Save current context
    mov [eax + 0],  ebx   ; Save general purpose registers
    mov [eax + 4],  ecx
    mov [eax + 8],  edx
    mov [eax + 12], esi   ; Save index registers
    mov [eax + 16], edi
    mov [eax + 20], ebp   ; Save base pointer
    
    pushf                  ; Get flags
    pop ecx
    mov [eax + 28], ecx   ; Save flags
    
    mov ecx, cr3
    mov [eax + 32], ecx   ; Save page directory base
    
    mov ecx, [esp]        ; Get return address
    mov [eax + 24], ecx   ; Save as EIP
    
    lea ecx, [esp + 4]    
    mov [eax + 36], ecx   ; Save ESP
    
load_new:
    ; Load new context
    mov eax, [esp + 8]    ; Get new_context
    
    ; Load new page directory if different
    mov ecx, [eax + 32]   ; Get new CR3
    mov edx, cr3
    cmp ecx, edx
    je .skip_cr3
    mov cr3, ecx
.skip_cr3:
    
    ; Load registers
    mov ebx, [eax + 0]    ; Restore general purpose registers
    mov ecx, [eax + 4]
    mov edx, [eax + 8]
    mov esi, [eax + 12]   ; Restore index registers
    mov edi, [eax + 16]
    mov ebp, [eax + 20]   ; Restore base pointer
    
    ; Load flags
    push dword [eax + 28]
    popf
    
    ; Set up new stack frame
    mov esp, [eax + 36]   ; Restore stack pointer
    push dword [eax + 24] ; Push return address (EIP)
    
    ret                   ; Jump to new context
