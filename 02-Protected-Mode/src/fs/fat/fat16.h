#ifndef FAT16_H
#define FAT16_H

#include "../file.h"
#include "../../string/string.h"
#include "../../status.h"
#include "../../disk/disk.h"
#include "../../disk/streamer.h"
#include "../../memory/heap/kheap.h"
#include "../../memory/memory.h"
#include <stdint.h>

struct filesystem *fat16_init();

#endif