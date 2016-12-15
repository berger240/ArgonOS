#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <multiboot.h>
#include <memdetection.h>
#include <descriptor_tables.h>
#include <timer.h>
#include <kernel/clock.h>
#include <kernel/terminal.h>
#include <kernel/monitor.h>
#include <atapio.h>
#include <ps2.h>
#include <mm.h>
#include <vm.h>



uint16_t data[256];

uint8_t buffer[2048];




void kernel_early(multiboot_info_t* mbd)
{
	descriptor_tables_init();
	terminal_init();
	ps2_init();
        detect_memory(mbd);
	IDE_init();
	init_timer(100);
	init_memory_allocator();
}



void kernel_main(void)
{
	//terminal_intro_logo();
        terminal_16color_demo();

	print_mmap();

	printf("Pages:%i\n",get_available_frames());
	printf("Mem:%i MB\n",get_available_frames()>>8);
	
	void *p1, *p2, *p3, *p4;
	printf("malloc:0x%X\n",(uint32_t)(p1=frame_malloc()));
	printf("malloc:0x%X\n",(uint32_t)(p2=frame_malloc()));
	printf("malloc:0x%X\n",(uint32_t)(p3=frame_malloc()));
	printf("malloc:0x%X\n",(uint32_t)(p4=frame_malloc()));
	
	frame_free(p3);
	frame_free(p1);
	frame_free(p2);
	frame_free(p4);

	printf("malloc:0x%X\n",(uint32_t)(p1=frame_malloc()));
	printf("malloc:0x%X\n",(uint32_t)(p2=frame_malloc()));
	printf("malloc:0x%X\n",(uint32_t)(p3=frame_malloc()));
	printf("malloc:0x%X\n",(uint32_t)(p4=frame_malloc()));

	frame_free(p3);
	frame_free(p1);
	frame_free(p2);
	frame_free(p4);

	//printf("%X\n",find_avaible_memory_over_1M());
/*	
	terminal_clr();
	print_ata_devices();
	printf("\n");

	int a = ata_identify(ATA_PRIMARY,ATA_MASTER, data);
	if(a==0){
		for(int i =0;i<128; i++){
			printf("%i:%i;  ",i,data[i]);
		}
	}

	printf("%i",a);
	
	terminal_clr();

	atapi_drive_read_sector (ATA_PRIMARY, ATA_MASTER ,0x10, buffer);
	for(int i =0;i<2048; i++){
		if(buffer[i]!=0){		
			putchar(buffer[i]);
		}else{
			putchar('-');
		}
	}

	terminal_clr();

	atapi_drive_read_sector (ATA_PRIMARY, ATA_MASTER ,0x11, buffer);
	for(int i =0;i<1024; i++){
		if(buffer[i]!=0){		
			putchar(buffer[i]);
		}else{
			putchar('-');
		}
	}*/

	for(;;);
}
