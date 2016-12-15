#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stddef.h>
#include <stdint.h>

struct gdt_entry_struct{
	uint16_t lower_limit;
	uint16_t lower_base;
	uint8_t middle_base;
	uint8_t access;
	uint8_t granularity;
	uint8_t higher_base;
}__attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t; 


struct gdt_ptr_struct
{
   uint16_t limit;               // The upper 16 bits of all selector limits.
   uint32_t base;                // The address of the first gdt_entry_t struct.
}
 __attribute__((packed));
typedef struct gdt_ptr_struct gdt_ptr_t; 


void init_gdt();

#endif
