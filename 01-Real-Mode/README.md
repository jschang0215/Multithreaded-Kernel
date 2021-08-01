# 01. 16bit Real Mode Development

Explained in https://jschang.tistory.com/35

## 서론
이번 글에서는 16bit real mode 개발에 대해 다루겠습니다. Bootloader, Segmentation, BPB, Interrupt Vector Table, Disk I/O에 대해 알아보겠습니다.

## 이론

### Bios & Bootloader
Bios는 ROM(Read Only Memory)에 상주하는 프로그램으로써 하드웨어들을 초기화/점검하고 Bootloader을 불러오며, Bootloader는 Kernel을 불러옵니다.

* 컴퓨터가 켜지면서 Bios는 스스로 RAM위에 올려져 실행됩니다.
    * Bios는 16bit code만 실행 가능
* Bios는 Bootloader를 불러오기 위해 모든 저장장치(하드디스크, USB, Floppu Disk etc )들의 첫 번째 Sector를 확인하면서 Boot Signature(0x55AA)가 있는지 확인합니다.
    * Sector: 저장 장치의 한 구역; 크기는 512byte
* Bios는 Bootloader을 RAM의 0x7c00 주소에 불러오고 Bios는 바로 0x7c00에서 프로세스를 시작하게 하여 Bootloader가 실행되게 합니다.

### Real Mode
Real Mode는 Compatibility Mode이다. Real Mode일 때는 다음과 같은 성질을 가집니다.

* 최대 1Mb의 Ram만 사용 가능
* Original x86 디자인 사용
    * Intel 8086 프로세서 다루듯이 사용
    * 16bit 사용
* 보안 기능 없음
* 한번에 16bit만 접근 가능 (최대 65545까지 접근 가능)

### Segmentation Memory Model
* 8086 Segment Register
    * CS: Code Segment
    * SS: Stack Segment
    * DS: Data Segment
* 절대 주소 계산 (Absolute Offset)
    * (Segment Register)*16 + offset
    * Ex) CS=0x7c0, Assembly ORG(offset)=0: (0x7c0*16)+0 = 0x7c00
* 서로 다른 Insturcution은 서로 다른 Segment 레지스터 사용
    * Ex) lodsb: DS:SI 레지스터 조합
* Stack Segment
    * Stack에 Push 할 때마다 SP(Stack Pointer) 2 감소

### BPB (Bios Parameter Block)
일부 Bios는 디스크에 BPB를 덮어써 bootsector 데이터가 손상되는 현상이 발생합니다. 따라서 이러한 작업을 하지 않도록 미리 BPB를 초기화해야 합니다. 그러기 위해서는 불필요한 BPB 부분들은 0으로 채워 넣으면 됩니다. 이때 첫 3byte는 코드가 있는 메모리로 점프해서 코드를 실행하는 역할을 하므로 필요하고, 나머지 33byte는 불필요해 0으로 초기화합니다.

### Interrupts
Interrupt는 코드를 메모리 주소로 부르는 대신 Interrupt Number로 호출하게 합니다. 예를 들어 interrupt 0x32를 특정 코드를 가리키게 한 다음 'int 0x32'를 하면 해당 interrupt 코드가 실행되게 됩니다.

Interrrupt을 발생하면

1. Processer가 interrupt 됨
2. Stack에 현재 상태가 저장
3. Interrupt 실행

### Interrupt Vector Table
Interrupt Vector Table은 Interrupt가 메모리 어디에 있는지 나타내는 table입니다.

* 각 항은 offset:segment(2byte + 2byte)로 된 4byte
* 각 항은 순서대로 정렬됨
### Disk Access
* 데이터는 Sector(512byte block)에 일고 씀
* CHS(Cylinder Head Sector): Sector 접근하기 위해 특정 head, track, sector 참조 (old)
* LBA(Logical Block Address): CHS와 달리 0부터 시작하는 수를 이용해 sector 참조 (current)
    * Ex) 디스크의 123456번째 위치 참조하려면 LBA=123456/512=241, offset=123456%512=64로 접근
* 16bit real mode에서는 Interrupt 13h로 디스크 접근 가능 설명
* 디스크 읽기
    * ah: 0x02
    * al: 읽을 sector 개수
    * ch: cylinder lower byte
    * cl: sector number
    * dh: head number
    * es:bs: data buffer
    * dl(drive number)는 자동으로 설정