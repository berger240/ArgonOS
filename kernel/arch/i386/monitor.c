#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/monitor.h>



static uint16_t* const video_memory = (uint16_t*) 0xC00B8000;

void monitor_set_char(int x, int y, char c, int bg_color, int t_color)
{
	uint16_t entry;
	entry=bg_color;
	entry=((entry<<4|t_color)<<8)|c;
	video_memory[y * MONITOR_WIDTH + x]=entry;
}

void monitor_get_char(int x, int y, char *c, int *bg_color, int *t_color)
{
	uint16_t entry;
	entry=video_memory[y * MONITOR_WIDTH + x];
	*c=entry&0xFF;
	*t_color=(entry>>8)&0xF;
	*bg_color=entry>>12;
}

void monitor_set_cursor(int x, int y)
 {
    unsigned short position=(y*MONITOR_WIDTH) + x;
 
    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));

 }


