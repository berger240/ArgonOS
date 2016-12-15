#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static void print(const char* data, size_t data_length)
{
	for ( size_t i = 0; i < data_length; i++ )
		putchar((int) ((const unsigned char*) data)[i]);
}

int printf(const char* restrict format, ...)
{
	va_list parameters;
	va_start(parameters, format);

	int written = 0;
	size_t amount;
	bool rejected_bad_specifier = false;

	while ( *format != '\0' )
	{
		if ( *format != '%' )
		{
		print_c:
			amount = 1;
			while ( format[amount] && format[amount] != '%' )
				amount++;
			print(format, amount);
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format;

		if ( *(++format) == '%' )
			goto print_c;

		if ( rejected_bad_specifier )
		{
		incomprehensible_conversion:
			rejected_bad_specifier = true;
			format = format_begun_at;
			goto print_c;
		}

		if ( *format == 'c' )
		{
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			print(&c, sizeof(c));
		}
		else if ( *format == 's' )
		{
			format++;
			const char* s = va_arg(parameters, const char*);
			print(s, strlen(s));
		}
		else if ( *format == 'd' || *format == 'i')
		{
			format++;
			long number = va_arg(parameters, long);
			char array[32];
			char *digits=(char *)array;

			if(number<0) putchar('-');

			if(number==0) {putchar('0'); continue;}
	
			number = abs(number);

	
			while(number!=0)
			{
		
				*(digits++)=number%10;
				number/=10;	
			}	
	
			while(digits!=array)
			{
				putchar('0'+*(--digits));
			}

		}
		else if ( *format == 'x' || *format == 'X')
		{
			int uord;
			if(*format == 'x') uord=0; 
			else uord=1;

			format++;
			unsigned int number = va_arg(parameters, unsigned int);
			char array[32];
			char *digits=(char *)array;

			if(number==0) {putchar('0'); continue;}
	
			while(number!=0)
			{
				*(digits++)=number%16;
				number/=16;	
			}	
	
			while(digits!=array)
			{
				if(uord) putchar(*("0123456789ABCDEF"+*(--digits)));
				else putchar(*("0123456789abcdef"+*(--digits)));
			}
		}else if ( *format == 'p'){


			format++;
			unsigned long number = (unsigned long)va_arg(parameters,void *);
			char array[32];
			char *digits=(char *)array;


			if(number==0) {putchar('0'); continue;}
	
			number = abs(number);
	
			while(number!=0)
			{
				*(digits++)=number%16;
				number/=16;	
			}	
	
			while(digits!=array)
			{
				putchar(*("0123456789abcdef"+*(--digits)));
			}
		}
		else
		{
			goto incomprehensible_conversion;
		}
		
	}

	va_end(parameters);

	return written;
}
