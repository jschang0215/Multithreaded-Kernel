#ifndef FILE_H
#define FILE_H

#include "pparser.h"

typedef unsigned int FILE_SEEK_MODE;
enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef unsigned int FILE_MODE;
enum
{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;

typedef void *(*FS_OPEN_FUNCTION)(struct disk *disk, struct path_part *path, FILE_MODE mode);
typedef int (*FS_RESOLVE_FUNCTION)(struct disk *disk);

struct filesystem
{
    // 디스크가 해당 파일시스템을 사용할 시 0 리턴
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;

    char name[20];
};

struct file_descriptor
{
    int index;
    struct filesystem *filesystem;

    // Privat data for internal file descriptor
    void *private;

    // file descriptor가 사용하는 디스크
    struct disk *disk;
};

void fs_init();
int fopen(const char *filename, const char *mode_string);
void fs_insert_filesystem(struct filesystem *filesystem);
struct filesystem *fs_resolve(struct disk *disk);

#endif