#ifndef _KERNEL_MONITOR_H
#define _KERNEL_MONITOR_H

#include <stddef.h>

enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

static const size_t MONITOR_WIDTH = 80;
static const size_t MONITOR_HEIGHT = 25;

void monitor_set_char(int x, int y, char c, int bg_color, int t_color);
void monitor_get_char(int x, int y, char *c, int *bg_color, int *t_color);

void monitor_set_cursor(int row, int col);
void terminal_16color_demo();
#endif
