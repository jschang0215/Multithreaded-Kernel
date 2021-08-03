ENTRY(__start)
OUTPUT_FORMAT(binary)
SECTIONS
{
    . = 1M; /* Kernel은 1Mb 크기로 올려짐 */
    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(COMMON)
        *(.bss)
    }
}