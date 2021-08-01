ORG 0 ; Offset 설정
BITS 16 ; 16bit 아키텍처 사용

; BPB 첫 3byte
__start:
    jmp short _start
    nop

; Bios Parameter Block 초기화 부분
times 33 db 0

_start:
    jmp 0x7c0:start ; Code Segment 0x7c0 설정하면서 start로 점프

handle_zero:
    mov ah, 0eh
    mov al, 'A'
    mov bx, 0x00
    int 0x10
    iret

handle_one:
    mov ah, 0eh
    mov al, 'B'
    mov bx, 0x00
    int 0x10
    iret

start:
    cli ; Interrupt Clear
    mov ax, 0x7c0
    mov ds, ax
    mov es, ax
    mov ax, 0x0
    mov ss, ax
    mov sp, 0x7c00 ; Stack 포인터 설정
    sti ; Interrupt 활성화

    mov ah, 02h
    mov al, 1 ; 읽을 sector 개수
    mov ch, 0 ; cylinder number lower byte
    mov cl, 2 ; sector number
    mov dh, 0 ; head number
    mov bx, buffer ; sector에서 읽은 내용 buffer label(메모리 주소)에 저장
    int 0x13

    jc error ; 에러 발생했을 시 carry flag 변화
    mov si, buffer
    call _print

    jmp $

error:
    mov si, error_message
    call _print
    jmp $ ; Infinte Loop

; 문자열 출력
print_char:
    mov ah, 0eh
    int 10h
    ret

_print:
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
error_message: db "Failed loading sector", 0

times 510-($-$$) db 0 ; 최소 512byte 돼야 함
dw 0xAA55 ; Little Endian에 의해 0x55AA 됨; 바로 binary로 저장 

buffer: