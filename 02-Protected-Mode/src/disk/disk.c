#include "../io/io.h"
#include "disk.h"
#include "../memory/memory.h"
#include "../config.h"
#include "../status.h"

struct disk disk; // Primagry Hard Disk

int disk_read_sector(int lba, int total, void *buf)
{
    // 1. master dirve를 선택하고 LBA 전달
    outb(0x1F6, (lba >> 24) | 0xE0);

    // 2. 읽을 sector 개수 전달
    outb(0x1F2, total);

    // LBA 설정
    outb(0x1F3, (unsigned char)(lba & 0xFF));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));

    // 3. 0x20(읽기 명령어)로 읽기 시작
    outb(0x1F7, 0x20);

    unsigned short *ptr = (unsigned short *)buf;
    for (int i = 0; i < total; i++)
    {
        // 4. buffer가 준비되기 까지 대기
        char c = insb(0x1F7);
        while (!(c & 0x08))
        {
            c = insb(0x1F7);
        }
        // 5. ATA controller에서 2byte씩 buffer에 읽음
        for (int j = 0; j < 256; j++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }
    return 0;
}

void disk_search_and_init()
{
    memset(&disk, 0, sizeof(disk));
    disk.type = DISK_TYPE_REAL;
    disk.sector_size = SECTOR_SIZE;
}

struct disk *disk_get(int index)
{
    if (index != 0)
    {
        return 0;
    }
    return &disk;
}

int disk_read_block(struct disk *idisk, unsigned int lba, int total, void *buf)
{
    if (idisk != &disk)
    {
        return -EIO;
    }
    return disk_read_sector(lba, total, buf);
}