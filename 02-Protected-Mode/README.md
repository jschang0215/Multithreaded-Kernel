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