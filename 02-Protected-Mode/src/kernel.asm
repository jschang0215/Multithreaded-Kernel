[BITS 32]
global _start ; Label이 public하게 사용될 수 있도록
extern kernel_main

CODE_SEG equ 0x08
DATA_SEG equ 0x10

_start:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, 0x00200000 ; Stack 포인터
    mov esp, ebp

    ; A20 Line 활성 (Address Bus의 A20 line은 default로 비활성화됨)
    in al, 0x92
    or al, 2
    out 0x92, al

    ; Remap master PIC
    mov al, 00010001b ; initialization mode
    out 0x20, al
    mov al, 0x20 ; master ISR은 Interrupt 0x20에서 시작
    out 0x21, al
    mov al, 000000001b
    out 0x21, al
    
    call kernel_main

    jmp $

times 512-($-$$) db 0