#ifndef _KERNEL_CLOCK_H
#define _KERNEL_CLOCK_H

#include <stddef.h>
#include <stdint.h>

void clock_read();
unsigned char clock_get_second();
unsigned char clock_get_minute();
unsigned char clock_get_hour();
unsigned char clock_get_day();
unsigned char clock_get_month();
unsigned int clock_get_year();


#endif
