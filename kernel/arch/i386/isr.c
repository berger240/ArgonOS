#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <io.h>

#include <mm.h>
#include <descriptor_tables.h>
#include <kernel/clock.h>
#include <kernel/terminal.h>
#include <kernel/monitor.h>

#include <isr.h>




uint16_t cursor_x=0, cursor_y=0;
int bg_color=0;
int t_color=5;




void isr_handler(uint32_t err,uint32_t intnumber)
{ 

  if(intnumber==14){
	page_fault(err);
  }else{
	printf("recieved interrupt-interrupt number: %i   err:%i\n",(int)intnumber,(int)err);
  }

}
/*
void isr_handler(uint32_t intnumber)
{ 

  printf("recieved interrupt-interrupt number: %i \n",(int)intnumber);


}
*/

isr_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

void irq_handler(uint32_t intnumber)
{
 //printf("recieved irq-interrupt number: %i " ,(int)intnumber);
   // Send an EOI (end of interrupt) signal to the PICs.
   // If this interrupt involved the slave.
   if (intnumber >= 40)
   {
       // Send reset signal to slave.
       outb(0xA0, 0x20);
   }
   // Send reset signal to master. (As well as slave, if necessary).
   outb(0x20, 0x20);

   if (interrupt_handlers[intnumber] != 0)
   {
       isr_t handler = interrupt_handlers[intnumber];
       handler();
   }
} 
