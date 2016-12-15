#include <stdio.h>

#if defined(__is_kernel)
#include <kernel/terminal.h>
#endif

int putchar(int ic)
{
	char c=(char)ic;
#if defined(__is_kernel)
	terminal_putc( c);
#else
	// TODO: You need to implement a write system call.
#endif
	return ic;
}
