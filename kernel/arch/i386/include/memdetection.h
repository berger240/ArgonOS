#ifndef _KERNEL_MEMDETECTION_H
#define _KERNEL_MEMDETECTION_H

#include <stddef.h>
#include <stdint.h>
#include <multiboot.h>
#include <stdbool.h>

#define MAX_MEMORYMAP 64
typedef struct{
	uint32_t base_addr_low;
	uint32_t base_addr_high;
	uint32_t length_low;
	uint32_t length_high;
	bool useable;
	bool next;
}mmap_entry_t;


void detect_memory(multiboot_info_t* mbd);

void print_mmap();

uint32_t find_avaible_memory_over_1M();

bool get_useable_block(int n,uint32_t *base_low,uint32_t *base_high,uint32_t *size_low,uint32_t *size_high);

#endif
