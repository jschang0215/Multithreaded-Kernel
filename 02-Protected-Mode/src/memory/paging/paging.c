#include "paging.h"
#include "../heap/kheap.h"
#include "../../status.h"

void paging_load_directory(uint32_t *directory);

static uint32_t *current_directory = 0;

struct paging_4gb_chunck *paging_new_4gb(uint8_t flags)
{
    uint32_t *directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
    int offset = 0;
    for (int i = 0; i < PAGING_TOTAL_ENTRIES_PER_TABLE; i++)
    {
        // directory의 각 원소는 다시 1024개의 원소를 가리킴
        uint32_t *entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_PER_TABLE);
        for (int j = 0; j < PAGING_TOTAL_ENTRIES_PER_TABLE; j++)
        {
            entry[j] = (offset + (j * PAGING_PAGE_SIZE)) | flags;
        }
        // directory의 다음 원소는 1024*4096 만큼 떨어져 있음
        offset += PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE;
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITABLE;
    }
    struct paging_4gb_chunck *chunck_4gb = kzalloc(sizeof(struct paging_4gb_chunck));
    chunck_4gb->directory_entry = directory;
    return chunck_4gb;
}

void paging_switch(uint32_t *directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

uint32_t *paging_4gb_chunck_get_directory(struct paging_4gb_chunck *chunck)
{
    return chunck->directory_entry;
}

bool paging_is_aligned(void *addr)
{
    return ((uint32_t)addr % PAGING_PAGE_SIZE) == 0;
}

// 주어진 virtual_address에 대한 directory와 table출력
int paging_get_indexes(void *virtual_address, uint32_t *directory_index_out, uint32_t *table_index_out)
{
    int res = 0;
    if (!paging_is_aligned(virtual_address))
    {
        res = -EINVARG;
        goto out;
    }

    // paging 구조 참조
    *directory_index_out = ((uint32_t)virtual_address / (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE));
    *table_index_out = ((uint32_t)virtual_address % (PAGING_TOTAL_ENTRIES_PER_TABLE * PAGING_PAGE_SIZE) / PAGING_PAGE_SIZE);
out:
    return res;
}

int paging_set(uint32_t *directory, void *virtual_address, uint32_t phyiscal_address_and_flags)
{
    int res = 0;
    if (!paging_is_aligned(virtual_address))
        return -EINVARG;

    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    res = paging_get_indexes(virtual_address, &directory_index, &table_index);

    if (res < 0)
        return res;

    uint32_t entry = directory[directory_index];
    uint32_t *table = (uint32_t *)(entry & 0b11111111111111111111000000000000); // Page Directory Entry에서 Address부분 추출
    table[table_index] = phyiscal_address_and_flags;

    return 0;
}