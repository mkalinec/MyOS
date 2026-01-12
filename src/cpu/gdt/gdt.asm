global gdt_flush

gdt_flush:
    ; rdi = pointer to gdt_ptr
    lgdt [rdi]               ; Load GDTR

    mov ax, 0x10             ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to reload CS
    push 0x08                    ; Kernel code selector
    lea rax, [rel flush_target]  ; Load address of label
    push rax
    lretq                        ; 64-bit far return

flush_target:
    ret