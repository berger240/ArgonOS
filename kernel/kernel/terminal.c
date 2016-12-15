#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/monitor.h>
#include <kernel/terminal.h>

#define D_BG_COLOR COLOR_BLACK
#define D_T_COLOR COLOR_LIGHT_GREY


uint16_t cursor_x, cursor_y;
int bg_color;
int t_color;

void terminal_init()
{
	bg_color=D_BG_COLOR;
	t_color=D_T_COLOR;
	terminal_clr();
}


void terminal_clr()
{
	int x, y;
	for(y=0;y<MONITOR_HEIGHT;y++)
	{
		for(x=0;x<MONITOR_WIDTH;x++)
		{
			monitor_set_char(x, y, ' ', bg_color, t_color);
		}		
	}

	cursor_x=0;
	cursor_y=0;
	
	monitor_set_cursor(cursor_x, cursor_y);
}

void terminal_set_bg_color(int col)
{
	bg_color=col;
}

void terminal_set_t_color(int col)
{
	t_color=col;
}

void terminal_set_color_default()
{
	t_color=D_T_COLOR;
	bg_color=D_BG_COLOR;
}

void terminal_scroll()
{
	char c;
	int r_bg_color, r_t_color;
	int x, y;
	for(y=0;y<MONITOR_HEIGHT-1;y++)
	{
		for(x=0;x<MONITOR_WIDTH;x++)
		{
			monitor_get_char(x, y+1, &c, &r_bg_color, &r_t_color);			
			monitor_set_char(x, y, c, r_bg_color, r_t_color);
		}		
	}

	cursor_y++;

	for(x=0;x<MONITOR_WIDTH;x++)
	{			
		monitor_set_char(x, y, ' ', bg_color, t_color);
	}

	cursor_x=0;
	cursor_y=24;
	monitor_set_cursor(cursor_x, cursor_y);
}

void terminal_putc(char c)
{


	if(c==0x08 && cursor_x>0) //ošetření Backspace
	{
		cursor_x--;
	}

	if(c==0x09) //ošetření TAB
	{
		cursor_x+=8;
		cursor_x &= ~7;
	}

	else if(c=='\n') //ošetření zalomení řádku
	{
		cursor_x=0;
		cursor_y++;
	}
	
	else if(c=='\r') //ošetření návratu vozíku
	{
		cursor_x=0;
	}

	else if(c>=0x20)//Tisknutelné znaky
	{
		monitor_set_char( cursor_x, cursor_y, c, bg_color, t_color);

		if(cursor_x<80)//posun kurzoru
		{
			cursor_x++;
		}else{
			cursor_y++;
			cursor_x=0;
		}
		
	}

	if(cursor_y>=25)
	{
		terminal_scroll();
		cursor_y=24;
	}

	monitor_set_cursor(cursor_x,cursor_y);
	
}

void terminal_intro_logo(){
	printf("Welcome to the\n");
	terminal_set_t_color(COLOR_LIGHT_BLUE);
	printf("                                                          ________     _____    \n");
	printf("      /\\                                                 /  ____  \\   / ___ \\   \n");
	printf("     /  \\       _  __    _____    ______    _  ____      | /    \\ |  / /   \\_|  \n");
	printf("    / /\\ \\     | |/__|  / __  |  /  __  \\  | |/__  \\     | |    | |  \\ \\_____   \n");
	printf("   / /__\\ \\    |  /    / /  | |  | /  \\ |  |  /  | |     | |    | |   \\_____ \\  \n");
	printf("  / ______ \\   | |     | |  | |  | |  | |  | |   | |     | |    | |  __     \\ \\ \n");
	printf(" / /      \\ \\  | |     \\ \\__| |  | \\__/ |  | |   | |     | \\____/ |  \\ \\____/ / \n");
	printf("/_/        \\_\\ |_|      \\____ |  \\______/  |_|   |_|     \\________/   \\______/  \n");
	printf("                        _   | |                                                 \n");
	printf("                       \\ \\__/ /                                                \n");
	printf("                        \\____/                                                  \n");
	terminal_set_color_default();

}

void terminal_16color_demo(){
	int i = 0;
	for(;i<16;i++)
	{
		terminal_set_bg_color(i);
		terminal_set_t_color(15-i);
		printf(" %x ",i);
	}

	terminal_set_color_default();
	printf("\n");
}


