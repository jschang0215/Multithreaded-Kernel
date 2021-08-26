ORG 0x7c00 ; Offset 설정
BITS 16 ; 16bit 아키텍처 사용

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; BPB 첫 3byte
jmp short _start
nop

; FAT16 Header
ODEMIdentifier      db 'ChangOS ' ; OEM Identifier는 8byte
BytesPerSector      dw 0x200 ; 512byte Per Sector
SectorPerCluster    db 0x80
ReservedSectors     dw 200 ; Kernel 코드가 있는 영역
FATCopies           db 0x02
RootDirEntries      dw 0x40
NumSectors          dw 0x00
MediaType           db 0xF8
SectorsPerFat       dw 0x100
SectorsPerTrack     dw 0x20
NumberOfHeads       dw 0x40
HiddenSectors       dd 0x00
SectorsBig          dd 0x773594

; Extended BPB (Dos 4.0)
DriveNumber         db 0x80
WinNTBit            db 0x00
Signature           db 0x29
VolumeID            dd 0xD105
VolumeIDString      db 'ChangOSBOOT'
SystemIDString      db 'FAT16   '

_start:
    jmp 0:start ; Code Segment 0 설정하면서 start로 점프

start:
    cli ; Interrupt Clear
    mov ax, 0x00
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00 ; Stack 포인터 설정
    sti ; Interrupt 활성화

.load_protected:
    cli
    lgdt[gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:load32

; GDT
gdt_start:
gdt_null:
    dd 0x0
    dd 0x0

; offset 0x8
gdt_code: ; CS 가 가리킴
    dw 0xffff ; Segment limit (0-15)
    dw 0x00 ; Base Address (16-31)
    db 0x00 ; Base Address (0-7)
    db 0x9a ; Flag (8-15)
    db 11001111b ; Flag+Segment Limit (16-23)
    db 0x0 ; Base Adderss (24-31)

; offset 0x10
gdt_data: ; DS, SS, ES, FS, GS
    dw 0xffff ; Segment limit (0-15)
    dw 0x00 ; Base Address (16-31)
    db 0x00 ; Base Address (0-7)
    db 0x92 ; Flag (8-15)
    db 11001111b ; Flag+Segment Limit (16-23)
    db 0x0 ; Base Adderss (24-31)
gdt_end:

gdt_descriptor:
    dw gdt_end-gdt_start-1 ; Size
    dd gdt_start ; Offset

[BITS 32]
load32:
    mov eax, 1 ; Sarting Sector
    mov ecx, 100 ; Number of Sector
    mov edi, 0x0100000 ; Load할  Address 1Mb
    call ata_lba_read ; 메모리에 Sedtor load

    jmp CODE_SEG:0x0100000

ata_lba_read:
    mov ebx, eax ; Backup LBA

    ; hard disk controller에 lba의 가장 높은 8bit 전송
    shr eax, 24 ; eax 오른쪽으로 24만큼 shift (8bit 남음)
    or eax, 0xE0 ; Select master drive
    mov dx, 0x1F6 ; write할 port
    out dx, al

    ; 읽을 총 sector 전송
    mov eax, ecx
    mov dx, 0x1F2
    out dx, al

    ; 남은 기타 LBA bit 전송
    mov eax, ebx ; Backup LBA
    mov dx, 0x1F3
    out dx, al

    ; 남은 기타 LBA bit 전송
    mov eax, ebx ; Backup LBA
    mov dx, 0x1F4
    shr eax, 8
    out dx, al

    ; hard disk controller에 lba의 가장 높은 16bit 전송
    mov eax, ebx
    shr eax, 16 ; eax 오른쪽으로 24만큼 shift (8bit 남음)
    mov dx, 0x1F5 ; write할 port
    out dx, al

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al

; 메모리에 전체 sector 읽음
.next_sector:
    push ecx

; 다시 읽어야 할지 check
.try_again:
    mov dx, 0x1F7
    in al, dx
    test al, 8
    jz .try_again

    ; 한번에 256word 읽음
    mov ecx, 256
    mov dx, 0x1F0
    rep insw ; dx에 있는 IO port에 있는 내용을 es:di에 write
    pop ecx
    loop .next_sector

    ret

times 510-($-$$) db 0 ; 최소 512byte 돼야 함
dw 0xAA55 ; Little Endian에 의해 0x55AA 됨; 바로 binary로 저장 