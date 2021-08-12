#include "pparser.h"
#include "kernel.h"
#include "../string/string.h"
#include "../memory/heap/kheap.h"
#include "../memory/memory.h"
#include "../status.h"

static int path_valid_format(const char *filename)
{
    int len = strnlen(filename, MAX_PATH);
    // valid format ex: 0:/directory/file.file
    return (len >= 3 && is_digit(filename[0]) && memcmp((void *)&filename[1], ":/", 2) == 0);
}

static int get_drive_by_path(const char **path)
{
    if (!path_valid_format(*path))
    {
        return -EBADPATH;
    }
    int drive_no = to_numeric_digit(*path[0]);

    // 0:/에서 :/ 건너띄기 위해 +3해 path 인덱스 재설정 ex: 0:/file -> file
    *path += 3;
    return drive_no;
}

static struct path_root *create_root(int drive_number)
{
    struct path_root *path_r = kzalloc(sizeof(struct path_root));
    path_r->drive_no = drive_number;
    path_r->first = 0;
    return path_r;
}

// 서로 다른 path 추출 ex: bin/bash.bin에서 bin, 두번째 호출시 bash.bin 추출
static const char *get_path_part(const char **path)
{
    char *result_path_part = kzalloc(MAX_PATH);
    int i = 0;
    while (**path != '/' && **path != 0x00)
    {
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }
    if (**path == '/')
    {
        *path += 1;
    }
    if (i == 0)
    {
        kfree(result_path_part);
        result_path_part = 0;
    }
    return result_path_part;
}

// path에서 parse한 후 last_part에 추가
struct path_part *parse_path_part(struct path_part *last_part, const char **path)
{
    const char *path_part_str = get_path_part(path);
    if (!path_part_str)
    {
        return 0;
    }
    struct path_part *part = kzalloc(sizeof(struct path_part));
    part->part = path_part_str;
    part->next = 0x00;
    if (last_part)
    {
        last_part->next = part;
    }
    return part;
}

void path_parser_free(struct path_root *root)
{
    struct path_part *part = root->first;
    while (part)
    {
        struct path_part *next_part = part->next;
        kfree((void *)part->part);
        kfree(part);
        part = next_part;
    }
    kfree((void *)root);
}

struct path_root *pathparser_parse(const char *path, const char *current_directory_path)
{
    int res = 0;
    const char *tmp_path = path;
    struct path_root *root_path = 0;
    if (strlen(path) > MAX_PATH)
    {
        goto out;
    }

    res = get_drive_by_path(&tmp_path);
    if (res < 0)
    {
        goto out;
    }

    root_path = create_root(res);
    if (!root_path)
    {
        goto out;
    }

    struct path_part *first_part = parse_path_part(NULL, &tmp_path);
    if (!first_part)
    {
        goto out;
    }

    root_path->first = first_part;
    struct path_part *part = parse_path_part(first_part, &tmp_path);
    while (part)
    {
        part = parse_path_part(part, &tmp_path);
    }

out:
    return root_path;
}