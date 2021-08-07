#ifndef IO_H
#define IO_H

// port에서 읽음
unsigned char insb(unsigned short port);
unsigned short insw(unsigned short port);

// port에 val write
void outb(unsigned short port, unsigned char val);
void outw(unsigned short port, unsigned short val);

#endif