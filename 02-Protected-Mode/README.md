# 02. 32bit Protected Mode Development

Explained in https://jschang.tistory.com/36

## 서론
이번 글에서는 32bit protected mode 진입에 대해 다루겠습니다.

## 이론
### Protected Mode
Protected Mode에서는 다음 기능을 지원합니다.

* 메모리&하드웨어 보호
    * 각 프로세스에 대한 접근 제한 설정 가능 (Ring 0, Ring 1, ...; Ring 0: Kernel)
* 다양한 메모리 Scheme
    * Segmentation 레지스터가 차지하는 메모리 범위와 접근 권한을 담은 특정 자료구조를 가리킴
    * Paging: 가상 주소가 실제 주소를 가리키게 함
* 4Gb 메모리 사용 가능 (Real Mode에서는 1Mb만 사용 가능 했음)

Protected Mode로 진입하는 순서는 다음과 같습니다.
1. Interrupt 비활성화
2. GDT Load
3. CPU의 control 레지스터 cr0에 값 설정
4. Segment 레지스터 값 설정
5. Stack 설정

### GDT (Global Descriptor Table)
GDT에 대한 자세한 설명은 09. GDT를 참고하시면 됩니다. Real Mode에서는 Segment 레지스터를 이용해 메모리를 다루었지만, Protected Mode에서는 Segment가 시작되는 주소와 Segment의 크기를 담은 Segment Descriptor를 entry로 하는 Table인 GDT를 사용합니다. 또한 CPU에서 GDT에 접근할 때는 GDT의 크기와 GDT의 시작 주소를 담은 GDT Descriptor를 이용합니다.

GDT를 설정한 후에는 기존의 CS레지스터 값을 GDT Code Segment으로, Data Segment 레지스터(DS, SS, ES, FS, GS)은 GDT Data Segment으로 변경합니다.

### Cross Compiler Setting
Cross Compiler는 https://wiki.osdev.org/GCC_Cross-Compiler#Installing_Dependencies 을 보고 OS에 맞게 진행하시면 됩니다.

### Text Mode
Text Mode에서는 Ascii 문자를 VGA 메모리에 접근해 화면에 문자를 출력하게 합니다. VGA 메모리는 0xB8000부터 시작하며, 각 문자는 2byte(Ascii code, Color mode) 크기를 가집니다.

### IDT(Interrupt Descriptor Table)
IDT는 Protected Mode에서 Interrupt에 대한 정보를 담고 있습니다. IDT는 Interrupt Descriptor을 원소로 하는 비열로 이루어져 있으며, IDT의 n번째 인덱스 원소는 Interrupt n에 해당합니다.

* Offset (46-63): offset의 상위 주소
* P (47)
* DPL (45-46): Interrupt가 실행될 Ring Level
* S (44)
* Type (40-43): Gate Types
* 0 (32-39): Unused bits
* Selector (16-31): Interrupt의 Selector
* Offset (0-15): offset의 하위 주소

IDTR은 IDT를 가리키는 자료구조로 IDT의 주소(Base)와, IDT 크기 - 1(Limit)에 대한 정보를 가집니다. IDT를 부르려면 lidt 명령어를 이용합니다.

### Assembly 함수
Assembly에서 함수(함수 비슷한)를 선언하는 일반적인 format은 다음과 같습니다.
```assembly
function:
    push ebp
    mov ebp, esp
    ; 함수 첫번쨰 인자는 ebp+8, 두번째 인자는 ebp+12 ...
    ; 함수 내용
    pop ebp
    ret
```
다른 Assembly나 C 코드에서 함수를 호출할 때 인자가 전달되면, 함수에서 인자를 이용하기 위해서는 ebp+8을 첫번쨰 인자로 시작해 사용하면 됩니다. 이는 함수 사용시 Stack에 ebp, eip레지스터가 push되기 때문입니다.


예를 들어 인자가 2개인 함수 function을 function(2, 3)로 호출하고, function에서 push ebp까지 실행되면 Stack은 위 그림의 형태가 됩니다. 이때 eip는 함수가 호출된 코드, 즉 함수가 끝나면 돌아갈 주소를 저장하고 있습니다. 따라서 함수의 인자를 사용하려면 ebp+8부터 사용해야 합니다.

함수의 리턴값은 일반적으로 eax레지스터에 저장됩니다.

### Programmable Interrupt Controller
PIC는 하드웨어가 프로세서를 Interrupt할 수 있게 합니다. 이때 PIC는 master과 slave로 나뉩니다.

* IRQ 0~7을 처리하는 master
    * Control Port: 0x20 & 0x21
* IRQ 8~15을 처리하는 slave
    * Control Port: 0xA0 & 0xA1

IRQ는 Interrupt에 맵핑되어 있는데, 이때 default로 IRQ는 8~15번 Interrupt에 맵핑되어 있습니다. 하지만 Protected Mode에서 해당 Interrupt는 reserved 되어 있어 PIC를 다시 맵핑해줘야 합니다.

### Heap
Heap은 메모리에서 사용가능한 매우 큰 부분으로, Kernel에서 필요할 때 Heap에서 메모리를 할당받고(malloc) 사용을 완료하면 Heap에게 알립니다(free). Protected Mode에서는 32bit 메모리 주소에 접근 가능하므로, 최대 4.29Gb Ram에 접근할 수 있습니다. 설치된 메모리중 Heap은 하드웨어(VGA, 기타 장치)가 사용하지 않는 메모리를 가리킬 수 있으며, 본 강의에서는 Heap은 4098byte을 블록의 한 단위로 하여 다음과 같이 구현합니다.

* Enty Table: 어떤 메모리가 사용 중이고, 사용 가능한지 나타낸 Table; Table의 하나의 원소는 1byte 크기로 한 블록(4096byte) 표현
    * Entry 구조
        * Entry Type (0-3): 사용 가능/불가능 표현
        * Flags (4-7)
            * 0 (4-5): Unused
            * IS_FIRST (6): 메모리 할당에서 첫번째에 원소에 해당하는지
            * HAS_N (7): 배열에서 바로 왼쪽 옆 원소가 같은 메모리 할당에 해당하는지
* Data Pool: 사용가능한 메모리를 가리키는 포인터

다음은 메모리를 할당하는 과정(malloc)입니다.

1. malloc 함수에서 인자로 넘겨받은 size를 통해 할당할 블록 수를 계산 (블록 단위로만 할당 가능하기 때문)
2. Entry Table에서 사용가능한 상태인 블록 탐색
    * 11000001b: 사용 불가 & 첫번째 블록 & 옆 원소도 같은 메모리 할당임
    * 10000001b: 사용 불가 & 첫번째 블록 X & 옆 원소도 같은 메모리 할당임
    * 01000001b: 사용 불가 & 첫번째 블록 & 메모리 할당의 마지막 원소
    * 00000001b: 사용 불가 & 첫번째 블록 X & 메모리 할당의 마지막 원소
    * 00000000b: 사용 가능
3. 하나 이상의 블록이 필요하면 옆 블록도 사용가능한지 확인
4. 사용가능한 블록 찾으면 사용한다고 표시
5. 블록의 시작 주소 계산 Data pool 시작 주소 + (블록 번호)*(블록 크기)

다음은 메모리를 해제하는 과정(free)입니다.

1. free할 블록 개수 계산 (HAS_N bit으로 알 수 있음)
2. Entry Table에서 해당 블록을 0x00로 설정

### Paging
Paging은 메모리 주소를 재맵핑하여 다른 메모리 주소를 가리키도록 합니다. Paging은 4096byte 크기의 블록 단위로 이루어지며, 이러한 블록들을 page라고 합니다. Paging은 MMU(Memory Management Unit)에 의해 활성화되어, 가상 메모리주소(Virtual Address: 메모리 주소가 실제 접근하는 주소랑 다름)를 물리적 메모리 주소(Physical Address: 메모리 주소가 실제 접근하는 주소와 동일)로 연결합니다.

Paging의 구조는 Page Directories -> Page Table -> Memory순으로 이루어집니다.

* 1024개의 Page Directories는 각각 1024개의 Page Table을 가리킨다.
    * Page Directory는 Page Table에 대한 포인터, 설정을 담고 있다.
* 1024개의 Page Table은 각각 4096byte의 메모리를 담당한다.
* 각 4096byte의 메모리를 page라 한다.

Paging을 이용할 경우의 장점은 다음과 같습니다.

* 각 프로세스는 같은 가상 메모리 주소를 가리키면서 사용 가능 (이떄 서로 곂쳐서 사용되지 않음)
* 가상 메모리 주소를 사용함으로서 다른 프로세스가 사용하는 메모리를 알 수 없게 해 보안이 강화
* 프로그램의 코드에 데이터를 덮어씌우는 것과 같은 취약점을 방지할 수 있음

### PCI IDE Controller
IDE는 ATA 드라이브를 다른 장치와 연결하는 케이블입니다. IDE는 다음 4개의 드라이브를 연결할 수 있습니다.

* ATA (Serial): modern hard dirves
* ATA (Parallel): hard drives
* ATAPI (Serial): modern optical drives
* ATAPI (Parallel): optical drives

ATA 드라이브에서 읽는 과정은 다음과 같습니다.

1. master dirve를 선택하고 LBA 전달
2. 읽을 sector 개수 전달
3. 0x20(읽기 명령어)로 읽기 시작
4. buffer가 준비되기 까지 대기
5. ATA controller에서 2byte씩 buffer에 읽음

### Filesystem
파일 시스템은 디스크에 정보가 어떻게 저장되어 있는지 나타내는 구조입니다. 운영체제는 파일 시스템의 구조를 이용해 디스크에서 파일을 읽을 수 있습니다.

파일 시스템을 이해하기 위해서는 디스크에 대해 이해해야 합니다. 디스크는 sector를 단위로 분리된 정보의 배열에 불과합니다. 각 sector는 LBA 번호로 메모리에서 읽을 수 있으며, 우리가 부르는 '파일'은 디스크에 '존재'하는 것이 아닌, 파일 시스템에 의해 구현되는 것입니다.

파일 시스템의 파일 시스템 구조 헤더는 디스크에 있는 파일 개수, root 디렉터리 위치 등을 저장합니다. 파일 시스템은 FAT 16, FAT32, NTFS 등의 여러 종류가 있는데, 본 강의에서는 FAT 16에 대해 다룹니다. 다음은 FAT 16(File Allocation Table)의 구조입니다.

* 첫 번째 Sector: Boot Sector & Reserved Sector 개수 나타냄
* Reserved Sector: 파일 시스템이 무시하는 Sector
* 첫 번째 File Allocation Table: 사용 중 가능한 디스크 표시하는 Table
* 두 번째 File Allocation Table: Optional
* Root Directory: Root Directory에 어떤 파일/디렉터리가 있는지 나타냄; 파일/디렉터리 이름, 특성(ex Read Only), 디스크에서의 위치에 대한 정보 담음
* Data Region: 데이터가 존재하는 공간

### File Allocation Table (FAT)
FAT16은 cluster로 데이터와 하위 디렉터리를 표현합니다. 이때 각 cluster는 정해진 수의 sector을 이용하는데, FAT 16은 하나의 데이터에 대해 최소한 하나의 cluster를 사용하기 때문에 저장 공간이 낭비되는 단점이 있습니다. FAT 16은 최대 2Gb 크기의 파일을 지원합니다. 다음은 FAT 16의 디스크 레이아웃과 크기입니다.

* Boot Sector: 512byte
* Reserved Sector: Fat_header.reserved_sectors * 512 (Kernel 코드)
* FAT1: Fat_header.sectors_per_fat * 512
* FAT2: Fat_header.sectors_per_fat * 512 (Optional; 백업으로 사용)
* Root Directory: Fat_header.root_dir_entries * sizeof(struct fat_directory_item)
* Data Clusters

**FAT** FAT16에서 각 Entry는 2byte 크기이고 data cluster의 cluster의 사용 가능 여부를 표현합니다. 만약 2개 이상의 cluster가 필요한 파일의 경우, FAT는 다음 cluster에 대한 정보를 가지고 있어 마치 linked-list처럼 cluster를 표현하며, 해당 파일의 마지막 cluster일 경우, 0xffff을 데이터로 합니다.

![FAT16](https://www.pctechguide.com/wp-content/uploads/2011/09/31fat.gif)

위 그림에서 FAT의 인덱스 2번은 data cluster에서 2번째 cluster를 가리키며, FAT의 인덱스 2번의 데이터 3은 해당 파일이 FAT의 인덱스 3번이 가리키는 cluster도 사용하는 것을 의미합니다. FAT의 인덱스 3의 데이터 4에 의해 해당 파일은 FAT의 인덱스 4번이 가리키는 cluster도 사용하며, FAT의 인덱스 4번은 0xFFFF이므로, 해당 파일의 마지막 cluster임을 의미합니다. 즉, file1.txt는 data cluster의 2, 3, 4 cluster를 사용하는 것입니다. 

**Root Directry** 다음은 FAT16의 디렉터리 Entry입니다. FAT16의 디렉터리 Entry는 다음 내용을 가집니다.

* Filename (8byte; 사용하지 않는 부분은 Space)
* Extension (3byte; 사용하지 않는 부분은 Space)
* Attribute
    * 0x01: Read-only
    * 0x02: File hidden
    * 0x04: System file
    * 0x08: Volume label
    * 0x10: Subdirectory
    * 0x20: Archived
    * 0x40: Device
    * 0x80: Reserved
* Reserved
* Creation date, Last access
* First cluster bits
* Filesize