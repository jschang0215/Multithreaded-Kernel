#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_desc
{
    uint16_t offset_1; // offset의 하위 주소
    uint16_t selector; // Interrupt의 Selector
    uint8_t zero;      // Unused bits
    uint8_t type_attr;
    uint16_t offset_2; // offset의 상위 주소
} __attribute__((packed));

struct idtr_desc
{
    uint16_t limit; // IDT 크기 - 1
    uint32_t base;  // 주소
} __attribute__((packed));

void idt_init();

#endif