#ifndef _KERNEL_TIMER_H
#define _KERNEL_TIMER_H


#include <stddef.h>
#include <stdint.h>
#include <io.h>
#include <isr.h>
#include <kernel/terminal.h>
#include <kernel/monitor.h>

void init_timer(uint32_t frequency);

#endif
