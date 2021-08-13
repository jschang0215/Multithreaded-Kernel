#include "streamer.h"
#include "../memory/heap/kheap.h"
#include "../config.h"

struct disk_stream *disk_streamer_new(int disk_id)
{
    struct disk *disk = disk_get(disk_id);
    if (!disk)
    {
        return 0;
    }

    struct disk_stream *streamer = kzalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

int disk_streamer_seek(struct disk_stream *stream, int pos)
{
    stream->pos = pos;
    return 0;
}

int disk_streamer_read(struct disk_stream *stream, void *out, int total)
{
    int sector = stream->pos / SECTOR_SIZE;
    int offset = stream->pos % SECTOR_SIZE;
    char buf[SECTOR_SIZE];

    // disk에서 buf로 읽음
    int res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0)
    {
        goto out;
    }

    // 한번에 SECTOR_SIZE(512byte)만큼만 읽을 수 있음
    int total_to_read = 0;
    if (total > SECTOR_SIZE)
    {
        total_to_read = SECTOR_SIZE;
    }
    else
    {
        total_to_read = total;
    }

    // buf에 있는 내용 out으로 옮김
    for (int i = 0; i < total_to_read; i++)
    {
        *(char *)out++ = buf[offset + i];
    }

    // SECTOR_SIZE(512byte)보다 많이 읽어야 할 때
    stream->pos += total_to_read;
    if (total > SECTOR_SIZE)
    {
        res = disk_streamer_read(stream, out, total - SECTOR_SIZE);
    }

out:
    return res;
}

void disk_stremaer_close(struct disk_stream *stream)
{
    kfree((void *)stream);
}