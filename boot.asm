ORG 0x7c00 ; Offset 설정
BITS 16 ; 16bit 아키텍처 사용

start:
    call _print
    jmp $ ; Infinte Loop

; 문자열 출력
print_char:
    mov ah, 0eh
    int 10h
    ret

_print:
    mov si, message
    _loop:
        lodsb
        cmp al, 0
        je _done
        call print_char
        jmp _loop
    ret

_done:
    ret

message: db "Hello World", 0

times 510-($-$$) db 0 ; 최소 512byte 돼야 함
dw 0xAA55 ; Little Endian에 의해 0x55AA 됨; 바로 binary로 저장 