#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <mm.h>

page_directory_t main_page_directory;


void init_page_directory(page_directory_t * dir){
	for(int i = 0; i < 1024; i++){
		*(uint32_t *)(dir->entries[i])=0; 			       //Set every entry in directory as not present
		dir->pointers[i]=NULL;					       //Set all virtual address frame pointers as NULL pointers
	}
}


void init_main_page_directory(){
	init_page_directory(&main_page_directory);
	
}

void init_virtual_memory_manager(){
	init_main_page_directory();

}


void *create_virtual_address_space(){
	
}

/*-------------------------------------------------------------------------------------------------------------------*/
/*		         		      void *page_malloc()	        				     */
/* 	                         Returns pointer to begining of allocated 4kB page	                             */
/*-------------------------------------------------------------------------------------------------------------------*/

void *page_malloc(){
	
}


