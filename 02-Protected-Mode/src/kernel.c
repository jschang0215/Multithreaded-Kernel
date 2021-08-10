#include "kernel.h"
#include "idt/idt.h"
#include "io/io.h"
#include "memory/heap/kheap.h"
#include "memory/paging/paging.h"

uint16_t *video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

size_t strlen(const char *str)
{
    size_t len = 0;
    while (str[len])
    {
        len++;
    }
    return len;
}

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

    //setup paging
    kernel_chunck = paging_new_4gb(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(paging_4gb_chunck_get_directory(kernel_chunck));

    char *ptr = kzalloc(4096);
    paging_set(paging_4gb_chunck_get_directory(kernel_chunck), (void *)0x10000, (uint32_t)ptr | PAGING_ACCESS_FROM_ALL | PAGING_IS_PRESENT | PAGING_IS_WRITABLE);
    enable_paging();

    char *ptr2 = (char *)0x10000;
    ptr2[0] = 'A';
    ptr2[1] = 'B';
    print(ptr2);
    print(ptr);

    // enable interrupts
    enable_interrupts();
}