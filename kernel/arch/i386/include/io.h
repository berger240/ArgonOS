#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include <stddef.h>
#include <stdint.h>

//void outw(uint16_t port, uint16_t value);
void outw(uint32_t port, uint32_t value);
void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
extern void f_insw(uint16_t port,uint32_t buffer_address, uint32_t nuber_of_words);
#endif
