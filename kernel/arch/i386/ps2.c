#include <stddef.h>
#include <stdint.h>
#include <io.h>
#include <ps2.h>
#include <isr.h> 

static void keyboard_callback();
void ps2_write(uint8_t cmd);
void ps2_write2(uint8_t cmd, uint8_t data);

bool ps2_1_exist=false;
bool ps2_2_exist=false;

void ps2_write(uint8_t cmd){
	outb(0x64,cmd);
}

void ps2_write2(uint8_t cmd, uint8_t data){
	outb(0x64,cmd); //Send command
	while((inb(0x64) & 0x2) == 1); //Wait for controller to be ready to write
	outb(0x60,data); //Write data
}

uint8_t ps2_read(uint8_t cmd){
	outb(0x64,cmd); //Send command
	while((inb(0x64) & 0x1) == 0); //Wait for controller to be ready to read
	return inb(0x60);
}

void ps2_flush_buffer(){
	inb(0x60); 
	inb(0x60); 
	inb(0x60); 
	inb(0x60);
}

uint8_t ps2_init(){
	
	bool cant_be_dual_chanell = false;	


	ps2_write(0xAD); //Disable first PS/2 port 
	ps2_write(0xA7); //Disable second PS/2 port

	ps2_flush_buffer();

	uint8_t config_byte = ps2_read(0x20); //read config byte

	if((config_byte & 0x20) == 0) cant_be_dual_chanell = true;

	config_byte = config_byte & 0xBC; //clear bits 0,1 and 6
	
	ps2_write2(0x60,config_byte); //Write config byte 
	

	if(ps2_read(0xAA) != 0x55) {
		return 1;
	}

	if(!cant_be_dual_chanell){
		ps2_write(0xA8); //Enable second PS/2 port
		config_byte = ps2_read(0x20); //read config byte

		if(config_byte & 0x20 == 1){
			cant_be_dual_chanell = true;
		}else{
			ps2_write(0xA7); //Disable second PS/2 port
		}
	}

	if(ps2_read(0xAB) == 0x00) ps2_1_exist = true; //Send "Test first PS/2 port" command , If Controler passed set variable

	if(!cant_be_dual_chanell){
		if(ps2_read(0xA9) == 0x00) ps2_2_exist = true; //Send "Test second PS/2 port" command , If Controler passed set variable
	}

	if(ps2_1_exist){
                ps2_write(0xAE); //Enable first PS/2 port
		
		config_byte = ps2_read(0x20); //read config byte
		config_byte = config_byte | 0x1; 
		ps2_write2(0x60,config_byte); //Write config byte 
		//printf("%x\n",config_byte);

		ps2_flush_buffer();

		outb(0x60,0xFF); //Reset first PS/2 device 
		while(inb(0x64) & 0x1 == 0); //Wait for controller to be ready to read
		//printf("reset:%x\n",inb(0x60)); 	
		
		ps2_flush_buffer();

		outb(0x60,0xF4); //Reset first PS/2 device 
		while((inb(0x64) & 0x1) == 0); //Wait for controller to be ready to read
		//printf("scan:%x\n",inb(0x60)); //read config byte

		ps2_flush_buffer();

		outb(0x60,0xF0);
		while((inb(0x64) & 0x2) == 1); //Wait for controller to be ready to write
  		outb(0x60,0);
		while((inb(0x64) & 0x1) == 0); //Wait for controller to be ready to read
		//printf("scancode:%x\n",inb(0x60)); //read config byte
		while((inb(0x64) & 0x1) == 0); //Wait for controller to be ready to read
		//printf("scancode:%x\n",inb(0x60)); //read config byte




	}

	if(ps2_1_exist){
		ps2_write(0xA8); //Enable second PS/2 port
	}



	register_interrupt_handler(IRQ1, &keyboard_callback);

	while (inb(0x64) & 1) // wait until buffer is empty
	{
		inb(0x60);
	}

	return 0;
}

uint8_t getScancode()
{
    uint8_t scancode = 0;

    if (inb(0x64)&1)
        scancode = inb(0x60);   // 0x60: get scan code from the keyboard

    // ACK: toggle bit 7 at port 0x61
    uint8_t port_value = inb(0x61);
    outb(0x61, port_value |  0x80); // 0->1
    outb(0x61, port_value &~ 0x80); // 1->0

    return scancode;

}


static void keyboard_callback()
{
   uint8_t scancode = getScancode();
   //printf("Keylboard-%x\n",scancode);
   
}

