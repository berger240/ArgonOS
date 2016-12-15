#ifndef _KERNEL_ATAPIO_H
#define _KERNEL_ATAPIO_H

#include <stddef.h>
#include <stdint.h>
/*
#define ATA_PRIMARY 128
#define ATA_SECONDARY 0
#define ATA_SLAVE 16
#define ATA_MASTER 0
*/
#define ATA_PATA 3
#define ATA_SATA 4
#define ATA_PATAPI 1
#define ATA_SATAPI 2


/* The default and seemingly universal sector size for CD-ROMs. */
#define ATAPI_SECTOR_SIZE 2048
 
/* The default ISA IRQ numbers of the ATA controllers. */
#define ATA_IRQ_PRIMARY     0x0E
#define ATA_IRQ_SECONDARY   0x0F
 
/* The necessary I/O ports, indexed by "bus". */
#define ATA_DATA(x)         (x)
#define ATA_FEATURES(x)     (x+1)
#define ATA_SECTOR_COUNT(x) (x+2)
#define ATA_ADDRESS1(x)     (x+3)
#define ATA_ADDRESS2(x)     (x+4)
#define ATA_ADDRESS3(x)     (x+5)
#define ATA_DRIVE_SELECT(x) (x+6)
#define ATA_COMMAND(x)      (x+7)
#define ATA_DCR(x)          (x+0x206)   /* device control register */
 
/* valid values for "bus" */
#define ATA_PRIMARY     0x1F0
#define ATA_SECONDARY   0x170
/* valid values for "drive" */
#define ATA_MASTER    0xA0
#define ATA_SLAVE     0xB0
 
/* ATA specifies a 400ns delay after drive switching -- often
 * implemented as 4 Alternative Status queries. */
#define ATA_SELECT_DELAY(bus) \
  {inb(ATA_DCR(bus));inb(ATA_DCR(bus));inb(ATA_DCR(bus));inb(ATA_DCR(bus));}
 

void IDE_init();
uint8_t ata_identify(uint32_t ps,uint32_t ms, uint16_t *buf);
uint8_t detect_devtype(uint32_t ps,uint32_t ms);
void atapi_drive_read_sector(uint32_t ps, uint32_t ms, uint32_t lba,uint8_t *buffer);

void print_ata_devices();

#endif
