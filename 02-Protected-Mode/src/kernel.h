#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 20

#define MAX_PATH 108

void kernel_main();
void print();

#define ERROR(value) (void *)value
#define ERROR_I(value) (int)value
#define ISERR(value) ((int)value) < 0
#endif