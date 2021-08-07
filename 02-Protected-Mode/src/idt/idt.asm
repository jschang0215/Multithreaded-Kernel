section .asm

global idt_load
idt_load:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8] ; 함수의 첫번째 인자
    lidt [ebx]
    pop ebp
    ret