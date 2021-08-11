#ifndef CONFIG_H
#define CONFIG_H

#define TOTAL_INTERRUPTS 512

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define HEAP_SIZE_BYTES 104857600 // 100Mb size
#define HEAP_BLOCK_SIZE 4096
#define HEAP_ADDRESS 0x01000000       // https://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
#define HEAP_TABLE_ADDRESS 0x00007E00 // https://wiki.osdev.org/Memory_Map_(x86)#Overview

#define SECTOR_SIZE 512

#endif