#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"
#include "disk/disk.h"
#include "disk/streamer.h"
#include "fs/pparser.h"
#include "string/string.h"

uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char color)
{
    // Little Endian 때문에 메모리에 (문자, 색) 저장하려면 (색, 문자)로 입력해야 함
    return (color << 8) | c;
}

void terminal_putchar(int x, int y, char c, char color)
{
    video_mem[y * VGA_WIDTH + x] = terminal_make_char(c, color);
}

void terminal_writechar(char c, char color)
{
    if (c == '\n')
    {
        terminal_row++;
        terminal_col = 0;
        return;
    }
    terminal_putchar(terminal_col, terminal_row, c, color);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row++;
    }
}

void print(char *str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

void terminal_initialize()
{
    video_mem = (uint16_t *)(0xB8000);
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }
}

static struct paging_4gb_chunck *kernel_chunck = 0;
void kernel_main()
{
    terminal_initialize();
    print("Hello World\n");

    // initialize heap
    kheap_init();

    // intialize idt
    idt_init();

    // search and intialize disk
    disk_search_and_init();

    // setup paging
    kernel_chunck = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(paging_4gb_chunck_get_directory(kernel_chunck));

    // enable paging
    enable_paging();

    // enable interrupts
    enable_interrupts();

    struct disk_stream *stream = disk_streamer_new(0);
    disk_streamer_seek(stream, 0x201);
    unsigned char c = 0;
    disk_streamer_read(stream, &c, 1);
    while (true)
    {
        
    }
    
}