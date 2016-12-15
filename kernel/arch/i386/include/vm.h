#ifndef _KERNEL_VM_H
#define _KERNEL_VM_H

#include <stddef.h>
#include <stdint.h>

#define KERNEL_VIRTUAL_ADDRESS_OFFSET 0xC0000000
#define KERNEL_VIRTUAL_ADDRESS_TOP    0xFFFFFFFF

void init_virtual_memory_manager();



typedef struct{
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t wt         : 1;   // If the bit is set, write-through caching is enabled
   uint32_t cache      : 1;   // If the bit is set, the page will not be cached
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
   uint32_t dirty      : 1;   // Has the page been written to since last refresh?
   uint32_t unused     : 1;   // Schould be zero   
   uint32_t global     : 1;   // If set, prevents the TLB from updating the address in its cache if CR3 is reset.
   uint32_t notusednow : 3;   // Free for use
   uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct{
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t wt         : 1;   // If the bit is set, write-through caching is enabled
   uint32_t cache      : 1;   // If the bit is set, the page will not be cached
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
   uint32_t unused     : 1;   // Schould be zero
   uint32_t size       : 1;   // If set 4MB page if unset 4kB pages 
   uint32_t global     : 1;   // Ignored
   uint32_t notusednow : 3;   // Free for use
   uint32_t frame      : 20;  // Page Table/Frame address (shifted right 12 bits)
} page_directory_entry_t;

typedef struct
{
    page_t pages[1024];
} page_table_t;

typedef struct 
{
    page_directory_entry_t entries[1024]; 		   //Page directory entries
    void *pointers[1024];				   //Pointers to frame (Virtual Address)
} page_directory_t;

#endif
