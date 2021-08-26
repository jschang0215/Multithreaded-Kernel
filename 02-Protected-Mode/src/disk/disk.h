#ifndef DISK_H
#define DISK_H

#include "../fs/file.h"
#include "../io/io.h"
#include "../memory/memory.h"
#include "../config.h"
#include "../status.h"

typedef unsigned int DISK_TYPE;

#define DISK_TYPE_REAL 0 // Real Physcial Hard disk

struct disk
{
    DISK_TYPE type;
    int sector_size;
    int id;

    struct filesystem *filesystem;

    // private data of filesystem
    void *fs_private;
};

void disk_search_and_init();
struct disk *disk_get(int index);
int disk_read_block(struct disk *idisk, unsigned int lba, int total, void *buf);

#endif