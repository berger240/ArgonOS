#include <gdt.h>
#include <idt.h>
#include <descriptor_tables.h>
#include <timer.h> //pouze pro test


void descriptor_tables_init(){
	init_gdt();
	init_idt();
        asm volatile("sti");
}
