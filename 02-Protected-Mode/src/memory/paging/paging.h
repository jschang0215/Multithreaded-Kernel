#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PAGING_CACHE_DISABLED 0b00010000 // https://wiki.osdev.org/Paging#Page_Directory
#define PAGING_WRITE_THROUGH 0b00001000
#define PAGING_ACCESS_FROM_ALL 0b00000100
#define PAGING_IS_WRITABLE 0b00000010
#define PAGING_IS_PRESENT 0b00000001

#define PAGING_TOTAL_ENTRIES_PER_TABLE 1024
#define PAGING_PAGE_SIZE 4096

struct paging_4gb_chunck
{
    uint32_t *directory_entry;
};

struct paging_4gb_chunck *paging_new_4gb(uint8_t flags);
void paging_switch(uint32_t *directory);
void enable_paging();

int paging_set(uint32_t *directory, void *virtual_address, uint32_t phyiscal_address_and_flags);
bool paging_is_aligned(void *addr);

uint32_t *paging_4gb_chunck_get_directory(struct paging_4gb_chunck *chunck);

#endif