#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <memdetection.h>
#include <multiboot.h>
#include <stdbool.h>
#include <kernel/monitor.h>
#include <mm.h>

mmap_entry_t mmap[MAX_MEMORYMAP];

void detect_memory(multiboot_info_t* mbd){


	memory_map_t* mmap2 = (multiboot_info_t *)((uint32_t)(mbd->mmap_addr ) + (uint32_t)KERNEL_VIRTUAL_ADDRESS_OFFSET);

	memory_map_t* mmap2_adr = mmap2;

	int i = 0;


	while(mmap2 < (uint32_t)mmap2_adr + (uint32_t)mbd->mmap_length){
		mmap[i].base_addr_low = (uint32_t)mmap2->base_addr_low;
		mmap[i].base_addr_high = (uint32_t)mmap2->base_addr_high;
		mmap[i].length_low = (uint32_t)mmap2->length_low;
		mmap[i].length_high = (uint32_t)mmap2->length_high;
		mmap[i].useable=(mmap2->type == 1);
                mmap[i].next=true;
		i++;
		mmap2 = (memory_map_t*) ( (unsigned int)mmap2 + mmap2->size + sizeof(mmap2->size) );
	}
	mmap[i].next=false;
	
}


bool get_useable_block(int n,uint32_t *base_low,uint32_t *base_high,uint32_t *size_low,uint32_t *size_high){
	int i=0;	
	do{
		if(mmap[i].useable){
			if(n==0){
				*base_low=mmap[i].base_addr_low;
				*base_high=mmap[i].base_addr_high;
				*size_low=mmap[i].length_low;
				*size_high=mmap[i].length_high;
				return mmap[i].next;
			}
			
			n--;
		}
		i++;
	}while(mmap[i].next);

	*base_low=0;
	*base_high=0;
	*size_low=0;
	*size_high=0;
	return false;

}

int ndigits( unsigned long numb, int base){
	unsigned long long  a = base;
	int i = 1;

	while(numb >= a){
		a*=base;
		i++;
	}

	return i;
}

uint32_t find_avaible_memory_over_1M(){
	for(int i = 0;i<=MAX_MEMORYMAP && mmap[i].next;){
		if((  (mmap[i].base_addr_low + mmap[i].length_low) >= 0x100000 || mmap[i].base_addr_high>0) && mmap[i].useable){
			if(mmap[i].base_addr_high >0){
				return 0xFFFFFFFF;
			}else{
				return mmap[i].length_low;
			}
		}
		i++;
	}

	return 0;

}

void printadresshex(unsigned long high,unsigned long low){


	terminal_set_t_color(COLOR_LIGHT_BLUE);	

	if(high>0){
		for(int n = 0; n< 8-ndigits(high,16);n++) printf("0");
	        printf("%X",high);
	}else{
		printf("00000000");
	}
	terminal_set_t_color(COLOR_LIGHT_BROWN);
 	
	if(low>0){
		for(int n = 0; n< 8-ndigits(low,16);n++) printf("0");
		printf("%X",low);
		terminal_set_color_default();
                return;
	}
        printf("00000000");
	terminal_set_color_default();

        return;

}

void print_mmap(){
	int i = 0;
	uint64_t size;
	int d;
	printf("\n");
	printf("   |       BASE ADDRES       | EXACT SIZE [Byte]| SIZE | USED/UNUSED |\n");
	printf("---|------------------|------|------------------|------|-------------|\n");	
	while(mmap[i].next && i < MAX_MEMORYMAP){
		printf("%i.",i);
		for(int n = 0; n< 2-ndigits(i,10);n++) printf(" ");
		printf("|0x");

		printadresshex(mmap[i].base_addr_high,mmap[i].base_addr_low);
		printf("|");

		size=mmap[i].base_addr_low+(mmap[i].base_addr_high<<32);
		d=0;
		while(size>=1024){
			size/=1024;
			d+=1;
		}
		if(d==0) printf("%iB ",size);
		else if(d==1) printf("%ikB",size);
		else if(d==2) printf("%iMB",size);
		else printf("%iGB",size);
		for(int n = 0; n< 4-ndigits(size,10);n++) printf(" ");
		printf("|0x");

		printadresshex(mmap[i].length_high,mmap[i].length_low);
		printf("|");

		size=mmap[i].length_low+(mmap[i].length_high<<32);
		d=0;
		while(size>=1024){
			size/=1024;
			d+=1;
		}
		if(d==0) printf("%iB ",size);
		else if(d==1) printf("%ikB",size);
		else if(d==2) printf("%iMB",size);
		else printf("%iGB",size);
		for(int n = 0; n< 4-ndigits(size,10);n++) printf(" ");
		printf("|");

		if(mmap[i].useable){
		 	terminal_set_t_color(COLOR_GREEN);
			printf("UNUSED       ");
		        terminal_set_color_default();			
		}else{
		 	terminal_set_t_color(COLOR_RED);
			printf("USED         ");
		        terminal_set_color_default();	
		}
		printf("|\n");
 		i++;
	}
}
