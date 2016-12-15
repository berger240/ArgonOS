#ifndef _KERNEL_TERMINAL_H
#define _KERNEL_TERMINAL_H

#include <stddef.h>

void terminal_init();
void terminal_clr();
void terminal_scroll();
void terminal_putc(char c);
void terminal_set_bg_color(int col);
void terminal_set_t_color(int col);
void terminal_set_color_default();

void terminal_intro_logo();
void terminal_16color_demo();
#endif
