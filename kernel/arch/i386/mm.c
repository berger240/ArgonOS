#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <mm.h>
#include <vm.h>

#define STACK_OFFSET_MASK MEMORY_MAP_SIZE-1
#define MAP_OFFSET_MASK MEMORY_MAP_SIZE-1
#define STACK_NUMBER_SCHIFT 18
#define MAP_NUMBER_SCHIFT 18

uint8_t first_memory_map[MEMORY_MAP_SIZE];
void *first_memory_stack[MEMORY_MAP_SIZE];

uint8_t *memory_maps_list[MAX_MEMORY_BLOCKS];
void **memory_stacks_list[MAX_MEMORY_BLOCKS];

uint32_t stack_pointer;

int available_frames;							       //Hold value of available frames

void page_fault(uint32_t err_code){
   printf("PAGE FAULT: (");
   // A page fault has occurred.
   // The faulting address is stored in the CR2 register.
   uint32_t faulting_address;
   asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

   // The error code gives us details of what happened.
   int present   = !(err_code & 0x1); // Page not present
   int rw = err_code & 0x2;           // Write operation?
   int us = err_code & 0x4;           // Processor was in user-mode?
   int reserved = err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
   int id = err_code & 0x10;          // Caused by an instruction fetch?


   if (present) {printf("present ");}
   if (rw) {printf("read-only ");}
   if (us) {printf("user-mode ");}
   if (reserved) {printf("reserved");}
   printf(") at %i",faulting_address);
   printf("\n");
   abort();
	
}

void init_memory_map(uint8_t *memory_map,int n){
	memset(memory_map,MEM_UNUSEABLE,MEMORY_MAP_SIZE);
	uint32_t base_low;
	uint32_t base_high;
	uint32_t size_low;
	uint32_t size_high;
	uint32_t memory_map_block_base;
	uint32_t memory_map_block_size;

	for(int i = 0;get_useable_block(i,&base_low,&base_high,&size_low,&size_high);i++){
		memory_map_block_base=(base_high<<20) + (base_low>>12);
		memory_map_block_size=(size_high<<20) + (size_low>>12);

		if((uint32_t)memory_map_block_base>=MEMORY_MAP_SIZE*n && (uint32_t)memory_map_block_base<MEMORY_MAP_SIZE*(n+1)){
			memory_map_block_base-=n*MEMORY_MAP_SIZE;

			if(memory_map_block_base+memory_map_block_size<MEMORY_MAP_SIZE*(n+1)){
				memset((uint8_t *)((uint32_t)memory_map + memory_map_block_base),MEM_FREE,memory_map_block_size);
			}else if(memory_map_block_base<MEMORY_MAP_SIZE*(n+1)){
				memset((uint8_t *)((uint32_t)memory_map + memory_map_block_base),MEM_FREE,MEMORY_MAP_SIZE-(uint32_t)memory_map_block_base); /////???
			}
		}else if((uint32_t)memory_map_block_base<MEMORY_MAP_SIZE*n && ((uint32_t)memory_map_block_base+memory_map_block_size>MEMORY_MAP_SIZE*n)){
			if((uint32_t)memory_map_block_base+memory_map_block_size<MEMORY_MAP_SIZE*(n+1)){
				memset(memory_map,MEM_FREE,memory_map_block_size+(uint32_t)memory_map_block_base-MEMORY_MAP_SIZE*n);
			}else if(memory_map_block_base<MEMORY_MAP_SIZE*(n+1)){
				memset(memory_map,MEM_FREE,MEMORY_MAP_SIZE);
			}
		}

 	}

	memory_maps_list[n] = memory_map;
	

}

void *map_number_to_pointer(uint32_t map_number,uint32_t position){
	return(void *)(map_number*MEMORY_BLOCK_SIZE*1024+position*4096);
}

void set_address_in_memory_map(void *address,uint8_t val){
	uint32_t n = (uint32_t)address  >> (MAP_NUMBER_SCHIFT+12);
	uint32_t offset = ((uint32_t)address >> 12)  & MAP_OFFSET_MASK;
	memory_maps_list[n][offset] = val;
}

uint8_t get_address_in_memory_map(void *address){
	uint32_t n = (uint32_t)address  >> (MAP_NUMBER_SCHIFT+12);
	uint32_t offset = ((uint32_t)address >> 12)  & MAP_OFFSET_MASK;
	return memory_maps_list[n][offset];
}

void set_memory_stacks(){
	uint32_t n;
	uint32_t offset=0;
	stack_pointer=0;

	memory_stacks_list[0] = first_memory_stack;
	
	for(int j=0;(j<MAX_MEMORY_BLOCKS) && (memory_maps_list[j] != NULL);j++){
		for(uint32_t i = 0; i<MEMORY_MAP_SIZE ; i++){
			if(memory_maps_list[j][i]==MEM_FREE){
				n=stack_pointer>>STACK_NUMBER_SCHIFT;
				offset=stack_pointer & STACK_OFFSET_MASK;
				//printf("0x%x,",(uint32_t)map_number_to_pointer(j,i));
				memory_stacks_list[n][offset]=map_number_to_pointer(j,i);
				stack_pointer++;
			}
		}
	}

}



void not_enaugh_memory(){

}

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         		      void *frame_malloc()	        				     */
/* 	                         Returns pointer to begining of allocated 4kB frame	                             */
/*-------------------------------------------------------------------------------------------------------------------*/

void *frame_malloc(){
	if(stack_pointer!=0){
		stack_pointer--;
		uint32_t n=stack_pointer>>STACK_NUMBER_SCHIFT;
		uint32_t offset=stack_pointer & STACK_OFFSET_MASK;
		void *address = memory_stacks_list[n][offset];
		set_address_in_memory_map(address,MEM_ALLOCATED);
		return address;
	}else not_enaugh_memory();

	return NULL;
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         		      void frame_free(void *address)	        			     */
/* 	 Frees 4kB frame refered by address(first parameter) that points to begining of frame or to frame            */
/*-------------------------------------------------------------------------------------------------------------------*/

void frame_free(void *address){
	if(get_address_in_memory_map(address)==MEM_ALLOCATED){		       //If refered frame is allocated (solve twice freed problem)
		uint32_t n=stack_pointer>>STACK_NUMBER_SCHIFT;
		uint32_t offset=stack_pointer & STACK_OFFSET_MASK;
		memory_stacks_list[n][offset]=address;
		set_address_in_memory_map(address,MEM_FREE);
		stack_pointer++;					       //Increase stack pointer 
		available_frames++;					       //Increase available_frames
	}
}


/*-------------------------------------------------------------------------------------------------------------------*/
/*		         		      void init_memory_lists()						     */
/* 	 This internal functin sets all positions in physical memory manegment lists as empty(NULL pointers)	     */
/*-------------------------------------------------------------------------------------------------------------------*/

void init_memory_lists(){
	memset(memory_maps_list,NULL,MAX_MEMORY_BLOCKS);		       //Fill memory_maps_list with NULL pointers
	memset(memory_stacks_list,NULL,MAX_MEMORY_BLOCKS);		       //Fill memory_stack_list with NULL pointers
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         		   void allocate_kernel_frames()					     */
/* 	     Internal function that sets first KERNEL_FRAMES as used and protect kernel binary from overwrites       */
/*-------------------------------------------------------------------------------------------------------------------*/

void allocate_kernel_frames(){
	for(uint32_t i = 0;i<KERNEL_FRAMES;i++){			       //go from zero to KERNEL_FRAME
		memory_maps_list[0][i]=MEM_USED;			       //set first frames in first memory map as USED
	}
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         		init_physical_memory_allocator()					     */
/* 	     Function that is call at very beging of system loading it enables dynamic frame allocation	             */
/*					This function init virtual memory manager to                                 */
/*-------------------------------------------------------------------------------------------------------------------*/
void init_memory_allocator(){

	init_memory_lists();						       //Fill lists with NULL pointers

        //////////////////////////////////////////////////
	//     set memory map and stack for first GB	//
	//////////////////////////////////////////////////
	
	init_memory_map(first_memory_map,0);				       //Set memory flags geted from Bootloaders memory map
	allocate_kernel_frames();					       //Set frames used by kernel binary as USED
	set_memory_stacks(); 						       //Fill stack with free frames

	//////////////////////////////////////////////////
	// 	  Init virtual memmory manager		//
	//////////////////////////////////////////////////
	
	init_virtual_memory_manager();

        //////////////////////////////////////////////////
	//   Set memory maps and stacks for the rest	//
	//////////////////////////////////////////////////
	
	



	calc_available_frames();					       //calculate available frames to keep statistics of available memory

}

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         			calc_available_frames()						     */
/* 	     		Function that calculates nubmer of available frames in memory (slow method)	             */
/*-------------------------------------------------------------------------------------------------------------------*/

int calc_available_frames(){
	int n=0; 							       //Variable for counting free frames
	for(int j=0;(j<MAX_MEMORY_BLOCKS) && (memory_maps_list[j] != NULL);j++){ //Calc till end of memmory or to last non null list
		for(uint32_t i = 0; i<MEMORY_MAP_SIZE ; i++){		       //Go throw memmory map
			if(memory_maps_list[j][i]==MEM_FREE) n++;	       //if frame is labled as free increment n
		}
	}
	available_frames = n;
	return n;							       //Return available frames
}


/*-------------------------------------------------------------------------------------------------------------------*/
/*		         			get_available_frames()						     */
/* 	     					Return available frames	           				     */
/*-------------------------------------------------------------------------------------------------------------------*/

int get_available_frames(){
	return available_frames;					       //Return available frames
}


