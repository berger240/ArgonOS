#ifndef _KERNEL_MM_H
#define _KERNEL_MM_H

#include <stddef.h>
#include <stdint.h>
#include <vm.h>

/*-------------------------------------------------------------------------------------------------------------------*/
/*		  			Physical mememory manager defs          		    		     */
/*-------------------------------------------------------------------------------------------------------------------*/

#define MEMORY_BLOCK_SIZE 1024 * 1024           			       //size of memory block in kB , should be 4 * 2^n
#define MEMORY_MAP_SIZE MEMORY_BLOCK_SIZE / 4   			       //one for every 4kB page
#define MAX_MEMORY_BLOCKS 4		       				       //max 4GB RAM
#define KERNEL_FRAMES 1024*8						       //First 32MB of physical memory for kernel binary (MUST BE 1024*n)

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         		init_physical_memory_allocator()					     */
/* 	     Function that is call at very beging of system loading it enables dynamic frame allocation	             */
/*					This function init virtual memory manager to                                 */
/*-------------------------------------------------------------------------------------------------------------------*/

void init_memory_allocator();

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         		      void *frame_malloc()	        				     */
/* 	                         Returns pointer to begining of allocated 4kB frame	                             */
/*-------------------------------------------------------------------------------------------------------------------*/

void *frame_malloc();

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         		      void frame_free(void *)	        		    		     */
/* 	 Frees 4kB frame refered by address(first parameter) that points to begining of frame or to frame            */
/*-------------------------------------------------------------------------------------------------------------------*/

void frame_free(void *);

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         			get_available_frames()						     */
/* 	     					Return avaible frames	           				     */
/*-------------------------------------------------------------------------------------------------------------------*/

int get_available_frames();





void page_fault(uint32_t err_code);


void *frame_malloc();



typedef enum {
    MEM_UNUSEABLE=0,
    MEM_FREE=1,
    MEM_ALLOCATED=2,
    MEM_USED=3
} mem_state;

#endif
