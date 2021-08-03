[BITS 32]
global _start ; Label이 public하게 사용될 수 있도록
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

    jmp $

