
#include <stddef.h>
#include <stdint.h>
#include <atapio.h>
#include <isr.h>

uint16_t PM_info[256];
uint16_t PS_info[256];
uint16_t SM_info[256];
uint16_t SS_info[256];

volatile uint8_t ata_primary_irq=0;
volatile uint8_t ata_secondary_irq=0;

static void ata_primary_callback(){
	ata_primary_irq=1;
	//printf("ATA_PRIMARY_CALLBACK\n");
}

static void ata_secondary_callback(){
	ata_secondary_irq=1;
	//printf("ATA_SECONDARY_CALLBACK\n");
}


void IDE_init(){
	register_interrupt_handler(IRQ14, &ata_primary_callback);
	register_interrupt_handler(IRQ15, &ata_secondary_callback);

	ata_identify(ATA_PRIMARY,ATA_MASTER, PM_info);
	ata_identify(ATA_PRIMARY,ATA_SLAVE, PS_info);
	ata_identify(ATA_SECONDARY,ATA_MASTER, SM_info);
	ata_identify(ATA_SECONDARY,ATA_SLAVE, SS_info);
}



uint32_t get_adressable_sectors(uint16_t *info){
	return  info[61]<<16+info[60];
}

void ata_bus_reset(uint32_t ps){
	if(ps==ATA_PRIMARY){	
		outb(ATA_DCR(ATA_PRIMARY),4);
		ATA_SELECT_DELAY(ATA_PRIMARY);
		outb(ATA_DCR(ATA_PRIMARY),0);	
	}else{
		outb(ATA_DCR(ATA_SECONDARY),4);
		ATA_SELECT_DELAY(ATA_SECONDARY);
		outb(ATA_DCR(ATA_SECONDARY),0);	
	}
}

uint8_t ata_identify(uint32_t ps,uint32_t ms, uint16_t *buf){
	int type = detect_devtype(ps, ms);
	
	if(type == ATA_PATAPI || type == ATA_PATA){

		outb(ATA_DRIVE_SELECT(ps) , ms);

		if(ps==ATA_PRIMARY){	
			ATA_SELECT_DELAY(ATA_SECONDARY);
		}else{
			ATA_SELECT_DELAY(ATA_PRIMARY);
		}

		outb( ATA_SECTOR_COUNT(ps) , 0 ); //Set Sector Count to 0
		outb( ATA_ADDRESS1(ps) , 0 ); //Set Sector Number / LBAlo to 0
		outb( ATA_ADDRESS2(ps), 0 ); //Set Cylinder Low / LBAmid  to 0
		outb( ATA_ADDRESS3(ps) , 0 ); //Set Cylinder High / LBAhi  to 0

		if(type==ATA_PATA){
			outb(ATA_COMMAND(ps), 0xEC ); //Send identify device command
		}else if(type==ATA_PATAPI){
			outb(ATA_COMMAND(ps), 0xA1 ); //Send identify packet device command
		}

		if(inb(ATA_COMMAND(ps)) == 0) return 1; //Return 1 if driver doesnt exist

		while(inb(ATA_COMMAND(ps)) & 0x80); //Wait till BSY clears

		if(inb(ATA_ADDRESS3(ps))) return 2;  //Return 2 for non ATA device
		if(inb(ATA_ADDRESS2(ps))) return 2;  //Return 2 for non ATA device

		uint8_t val;
		while(!((val = inb(ATA_COMMAND(ps))) & 0x8)) //Wait till DRQ sets
		{
			if(val & 0x1) return 1; //Return 1 Error
		}

		for(int i = 0;i< 256; i++){
			buf[i]=inw(ATA_DATA(ps) );
		}
	}
	return 0;

}


void print_ata_device(uint32_t ps,uint32_t ms){
	int type=detect_devtype(ps,ms);
	if(type == ATA_PATAPI) printf("PATAPI"); 
	else if(type == ATA_SATAPI) printf("SATAPI"); 
	else if(type == ATA_PATA) printf("PATA"); 
	else if(type == ATA_SATA) printf("SATA");
	else printf("NONE OR UNKNOW"); 
}

void print_ata_devices(){
	printf("\nIDE Primary Master:");
	print_ata_device(ATA_PRIMARY,ATA_MASTER);
	printf(";%x",get_adressable_sectors(PM_info));
	printf("\nIDE Primary Slave:");
	print_ata_device(ATA_PRIMARY,ATA_SLAVE);
	printf(";%x",get_adressable_sectors(PS_info));
	printf("\nIDE Secondary Master:");
	print_ata_device(ATA_SECONDARY,ATA_MASTER);
	printf("\nIDE Secondary Slave:");
	print_ata_device(ATA_SECONDARY,ATA_SLAVE);
}



// on Primary bus: ctrl->base =0x1F0, ctrl->dev_ctl =0x3F6. REG_CYL_LO=4, REG_CYL_HI=5, REG_DEVSEL=6 

uint8_t detect_devtype(uint32_t ps,uint32_t ms)
{
	
	ata_bus_reset(ps);

	outb(ATA_DRIVE_SELECT(ps) , ms);

	if(ps==ATA_PRIMARY){	
		ATA_SELECT_DELAY(ATA_SECONDARY);
	}else{
		ATA_SELECT_DELAY(ATA_PRIMARY);
	}

	uint8_t cl=inb(ATA_ADDRESS2(ps));	// get the "signature bytes" 
	uint8_t ch=inb(ATA_ADDRESS3(ps));
 
	//differentiate ATA, ATAPI, SATA and SATAPI 
	if (cl==0x14 && ch==0xEB) return ATA_PATAPI;
	if (cl==0x69 && ch==0x96) return ATA_SATAPI;
	if (cl==0 && ch == 0) return ATA_PATA;
	if (cl==0x3c && ch==0xc3) return ATA_SATA;
	return 0;//UNKNOWN;
}


void atapi_drive_read_sector(uint32_t ps, uint32_t ms, uint32_t lba,uint8_t *buffer)
{   
    uint8_t read_pkt[12] = {0,0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0 }; 
 
    read_pkt[2] = (lba >> 0x18) & 0xFF;   // most sig. byte of LBA 
    read_pkt[3] = (lba >> 0x10) & 0xFF;
    read_pkt[4] = (lba >> 0x08) & 0xFF;
    read_pkt[5] = (lba >> 0x00) & 0xFF;   // least sig. byte of LBA 
    
    //read10 command
    read_pkt[0] = 0x28;
    read_pkt[7] = (1 >> 0x08) & 0xFF;      //msb transfer len
    read_pkt[8] = (1 >> 0x00) & 0xFF;   //lsb transfer len

   //atapi pkt req 6 words to be pushed not 12 bytes
   uint16_t read_pkt_w[6];
   int i,j;
   for(i=0,j=0;i<6;i++,j+=2)
   read_pkt_w[i] = read_pkt[j+1]<<8 | read_pkt[j];
    
   //Selecting up device to be read from
   outb (ATA_DRIVE_SELECT (ps),ms & (1 << 4));
   outb(ATA_FEATURES(ps),0);
       
   outb (ATA_ADDRESS2(ps), ATAPI_SECTOR_SIZE & 0xFF);
   outb (ATA_ADDRESS3(ps), ATAPI_SECTOR_SIZE >> 8);
    
    
   //ATATPI PKT cmd
   outb(ATA_COMMAND(ps),0xA0); //ATAPI_PKT_CMD
   //waiting for dev to get ready

   while((inb(ATA_COMMAND (ps)) & 0x80)
   && !(inb(ATA_COMMAND (ps)) & 0x08) );
   
   //pushing the packet to the data port
   for(i=0;i<6;i++)
      outw(ATA_DATA(ps),read_pkt_w[i]);
    
   while(!ata_primary_irq);
   
   ata_primary_irq=0;

    //transferring data
    for(i=0;i<ATAPI_SECTOR_SIZE/2;i++)
    {
      ((uint16_t*)buffer)[i] = inw (ATA_DATA (ps));
    }
}

/*


void atapi_drive_read_sector(uint32_t ps, uint32_t ms, uint32_t lba, uint32_t num_lba,uint8_t *buffer)
{   
    printf("1\n");
    uint8_t read_pkt[12] = {0,0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0 }; 
 
    read_pkt[2] = (lba >> 0x18) & 0xFF;   // most sig. byte of LBA 
    read_pkt[3] = (lba >> 0x10) & 0xFF;
    read_pkt[4] = (lba >> 0x08) & 0xFF;
    read_pkt[5] = (lba >> 0x00) & 0xFF;   // least sig. byte of LBA 
    
   if(num_lba > 0xFFFF)
   {
      //read12 command
      read_pkt[0] = 0xA8;
      read_pkt[6] = (num_lba >> 0x18) & 0xFF;      //msb transfer len
      read_pkt[7] = (num_lba >> 0x10) & 0xFF;
      read_pkt[8] = (num_lba >> 0x08) & 0xFF;
      read_pkt[9] = (num_lba >> 0x00) & 0xFF;      //lsb transfer len
   }
   else
   {
      //read10 command
      read_pkt[0] = 0x28;
      read_pkt[7] = (num_lba >> 0x08) & 0xFF;      //msb transfer len
      read_pkt[8] = (num_lba >> 0x00) & 0xFF;   //lsb transfer len
   }
   printf("2\n");
   //atapi pkt req 6 words to be pushed not 12 bytes
   uint16_t read_pkt_w[6];
   int i,j;
   for(i=0,j=0;i<6;i++,j+=2)
   read_pkt_w[i] = read_pkt[j+1]<<8 | read_pkt[j];
    
   //Selecting up device to be read from
   outb (ATA_DRIVE_SELECT (ps),ms & (1 << 4));
   outb(ATA_FEATURES(ps),0);
       
   outb (ATA_ADDRESS2(ps), ATAPI_SECTOR_SIZE & 0xFF);
   outb (ATA_ADDRESS3(ps), ATAPI_SECTOR_SIZE >> 8);
    
    
   //ATATPI PKT cmd
   outb(ATA_COMMAND(ps),0xA0); //ATAPI_PKT_CMD
   printf("3\n");
   //waiting for dev to get ready
   //while ((inb (ATA_COMMAND (ps))) & 0x88);
   
   while((inb(ATA_COMMAND (ps)) & 0x80)
   && !(inb(ATA_COMMAND (ps)) & 0x08) );
   
   //pushing the packet to the data port
   printf("4\n");
   for(i=0;i<6;i++)
      outw(ATA_DATA(ps),read_pkt_w[i]);
    
   //IRQ response wait***not implemented
   //while ((inb (ATA_COMMAND (ps))) & 0x88);
   //while((inb(ATA_COMMAND (ps)) & 0x80)
   //&& !(inb(ATA_COMMAND (ps)) & 0x08) );
   while(!ata_primary_irq);
   
   ata_primary_irq=0;

   //if(( inportb(ATA_STATUS_PORT(bus)) & ATA_STATUS_REG_ERR))kprintf("WTF JJJJJJJJ");
   
   //timer_wait(12);
   //a=true;
   //wait_for_interrupt();
   printf("5\n");
   //Actual size that is to transferred
   
          asm volatile("cli");
   uint32_t size =(((uint32_t) inb (ATA_ADDRESS3(ps))) << 8) | (uint32_t) (inb (ATA_ADDRESS2(ps)));
    uint16_t words = size/2;
    
    

    printf("\nActual Size to be transferred %i\n",(int)words);
    //transferring data
    //f_insw (ATA_DATA (ps),(uint16_t*)buffer, 1024);
    for(i=0;i<1024;i++)
    {
      ((uint16_t*)buffer)[i] = inw (ATA_DATA (ps));
   }
          asm volatile("sti");
}
*/

/* Use the ATAPI protocol to read a single sector from the given
 * bus/drive into the buffer using logical block address lba. */
/*
int atapi_drive_read_sector (uint32_t ps, uint32_t ms, uint32_t lba, uint8_t *buffer)
{

	// 0xA8 is READ SECTORS command byte. 
	uint8_t read_cmd[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t status;
	int size;
	// Select drive (only the slave-bit is set) 
	outb (ATA_DRIVE_SELECT (ps),ms & (1 << 4));
	//outb(ATA_DRIVE_SELECT(ps) , ms);      
	ATA_SELECT_DELAY (ps);       // 400ns delay 
	outb (ATA_FEATURES (ps), 0x0);       // PIO mode 
	outb (ATA_ADDRESS2 (ps),ATAPI_SECTOR_SIZE & 0xFF );
	outb (ATA_ADDRESS3 (ps),ATAPI_SECTOR_SIZE >> 8);
	outb (ATA_COMMAND (ps),0xA0);       // ATA PACKET command 


	while ((status = inb (ATA_COMMAND (ps))) & 0x80)     // psY 
	asm volatile ("pause");
	while (!((status = inb (ATA_COMMAND (ps))) & 0x8) && !(status & 0x1))
	asm volatile ("pause");
	// DRQ or ERROR set 
	if (status & 0x1) {
	size = -1;
	goto cleanup;
	}
	read_cmd[9] = 1;              //1 sector 
	read_cmd[2] = (lba >> 0x18) & 0xFF;   // most sig. byte of LBA 
	read_cmd[3] = (lba >> 0x10) & 0xFF;
	read_cmd[4] = (lba >> 0x08) & 0xFF;
	read_cmd[5] = (lba >> 0x00) & 0xFF;   // least sig. byte of LBA 
	// Send ATAPI/SCSI command 
	
	for(int i = 0;i<6;i+=1){
		outw(ATA_DATA (ps), (uint16_t *)read_cmd[i]);
		printf("outw:0x%X\n", (uint16_t *)read_cmd[i]);
	}

	// Wait for IRQ that says the data is ready. 
	printf("waiting for irq\n");
	if(ps==ATA_PRIMARY){
		//asm volatile ("int $0x2E"); 
		while(!ata_primary_irq) printf("1");
		ata_primary_irq=0;
	}else{
		while(!ata_secondary_irq);
		ata_secondary_irq=0;
	}
	printf("after irq\n");
	// Read actual size 
	size =
	(((int) inb (ATA_ADDRESS3 (ps))) << 8) |
	(int) (inb (ATA_ADDRESS2 (ps)));
	// This example code only supports the case where the data transfer
	// of one sector is done in one step. 
	//ASSERT (size == ATAPI_SECTOR_SIZE);  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Read data. 
	f_insw (ATA_DATA (ps), buffer, size / 2);
	printf("wait");
	while(inb(ATA_COMMAND(ps)) & 0x80); //Wait till BSY clears
	uint8_t val;
	while(!((val = inb(ATA_COMMAND(ps))) & 0x8)) //Wait till DRQ sets
	{
		if(val & 0x1) return 1; //Return 1 Error
	}

	// Wait for BSY and DRQ to clear, indicating Command Finished 
	printf("wait3");
	while ((status = inb (ATA_COMMAND (ps))) & 0x88) 
	asm volatile ("pause");
	cleanup:
	printf("exit");
	return size;
}
*/

/*

void atapi_read_pio(u32int bus, u32int drive, u32int lba, u32int num_lba,u8int *buffer)
{   
   u8int read_pkt[12] = {0,0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0 }; 
 
   read_pkt[2] = (lba >> 0x18) & 0xFF;   // most sig. byte of LBA 
    read_pkt[3] = (lba >> 0x10) & 0xFF;
    read_pkt[4] = (lba >> 0x08) & 0xFF;
    read_pkt[5] = (lba >> 0x00) & 0xFF;   // least sig. byte of LBA 
    
   if(num_lba > 0xFFFF)
   {
      //read12 command
      read_pkt[0] = ATAPI_READ12_CMD;
      read_pkt[6] = (num_lba >> 0x18) & 0xFF;      //msb transfer len
      read_pkt[7] = (num_lba >> 0x10) & 0xFF;
      read_pkt[8] = (num_lba >> 0x08) & 0xFF;
      read_pkt[9] = (num_lba >> 0x00) & 0xFF;      //lsb transfer len
   }
   else
   {
      //read10 command
      read_pkt[0] = ATAPI_READ10_CMD;
      read_pkt[7] = (num_lba >> 0x08) & 0xFF;      //msb transfer len
      read_pkt[8] = (num_lba >> 0x00) & 0xFF;   //lsb transfer len
   }
    
   //atapi pkt req 6 words to be pushed not 12 bytes
   u16int read_pkt_w[6];
   int i,j;
   for(i=0,j=0;i<6;i++,j+=2)
   read_pkt_w[i] = read_pkt[j+1]<<8 | read_pkt[j];
    
   //Selecting up device to be read from
   outportb(ATA_DRIVE_SELECT_PORT(bus),drive);
   outportb(ATA_FEATURES_PORT(bus),0);
       
   outportb (ATA_LBA_MID_PORT(bus), ATAPI_SECTOR_SIZE & 0xFF);
    outportb (ATA_LBA_HI_PORT(bus), ATAPI_SECTOR_SIZE >> 8);
    
    
   //ATATPI PKT cmd
   outportb(ATA_COMMAND_PORT(bus),ATAPI_PKT_CMD);
    
   //waiting for dev to get ready
   while((inportb(ATA_STATUS_PORT(bus)) & ATA_STATUS_REG_BSY)
   && !(inportb(ATA_STATUS_PORT(bus)) & ATA_STATUS_REG_DRQ) );
    
   //pushing the packet to the data port
   for(i=0;i<6;i++)
      outportw(ATA_DATA_PORT(bus),read_pkt_w[i]);
    
   //IRQ response wait***not implemented
   while((inportb(ATA_STATUS_PORT(bus)) & ATA_STATUS_REG_BSY)
   && !(inportb(ATA_STATUS_PORT(bus)) & ATA_STATUS_REG_DRQ) );
   if(( inportb(ATA_STATUS_PORT(bus)) & ATA_STATUS_REG_ERR))kprintf("WTF JJJJJJJJ");
   //timer_wait(12);
   //a=true;
   //wait_for_interrupt();
    
   //Actual size that is to transferred
   u32int size =
      (((u32int) inportb (ATA_LBA_HI_PORT (bus))) << 8) | (u32int) (inportb (ATA_LBA_MID_PORT (bus)));
     
    kprintf("\nActual Size to be transferred 0x%x",size);
     
    u16int words = size/2;
    //transferring data
    u16int temp;
    for(i=0,j=0;i<words;i++)
    {
      temp = inportw (ATA_DATA_PORT (bus));
      buffer[j++] = temp & 0xFF;
      buffer[j++] = (temp>>8) & 0xFF;
   }
}
*/

