#ifndef _ATA_H
#define _ATA_H

#include "types.h"

#define ATA_DATA			0x1F0
#define ATA_SECTOR_COUNT	0x1F2
#define ATA_LBA_LOW			0x1F3
#define ATA_LBA_MID			0x1F4
#define ATA_LBA_HIGH		0x1F5
#define ATA_DRIVE_SELECT	0x1F6
#define ATA_STATUS			0x1F7
#define ATA_MASTER_DRIVE	0xA0
#define ATA_SLAVE_DRIVE		0xB0
#define ATA_CMD_IDENTIFY	0xEC
#define ATA_BSY_MASK		0x80
#define ATA_DRQ_MASK		0x08
#define ATA_ERR_MASK		0x01
#define ATA_DF_MASK			0x20

#define ATA_MASTER_SLAVEBIT	0x0
#define ATA_SLAVE_SLAVEBIT	0x1
#define ATA_READ_MASTER		0xE0
#define ATA_READ_SLAVE		0xF0
#define ATA_CMD_READ		0x20
#define LBA_MODE_28			28

uint32_t ata_identify();
uint32_t ata_read_pio28(uint32_t sector, uint32_t sec_count, uint8_t* buf);
void ata_init();

#endif /* _ATA_H */
