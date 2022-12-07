#include "ata.h"
#include "types.h"
#include "lib.h"

/*
 * RETURN VALUE: 1 if has drive, 0 if drive does not exist
 * */
uint32_t ata_identify() {
	outb(ATA_MASTER_DRIVE, ATA_DRIVE_SELECT);
	outb(0, ATA_LBA_LOW);
	outb(0, ATA_LBA_MID);
	outb(0, ATA_LBA_HIGH);
	outb(ATA_CMD_IDENTIFY, ATA_STATUS);

	if (inb(ATA_STATUS) == 0) {
		return 0;
	} else {
		while (inb(ATA_STATUS) & ATA_BSY_MASK) {
			/* wait for BSY to be 0 */
		}
	}

	if (inb(ATA_LBA_MID) || inb(ATA_LBA_HIGH)) {
		return 0;
	} else {
		while(!(inb(ATA_STATUS) & ATA_DRQ_MASK ||
			inb(ATA_STATUS) & ATA_ERR_MASK)) {
			/* wait for ERR or DRQ to be set */
		}
	}

	if (inb(ATA_STATUS) & ATA_ERR_MASK) {
		return 0;	/* error */
	} else {
		return 1;
	}
	
	return 0;
}

/* Read 512 bytes to given buffer
 * RETURN VALUE: 1 if successful, 0 if error
 * */
uint32_t _read_to_buf_pio28(uint8_t* buf) {
	uint32_t i;
	uint16_t data;

	if (NULL == buf) {
		return 0;
	}

	/* Read the whole 512-byte sector */
	for (i = 0; i < 256; i++) {
		data = inw(ATA_DATA);
		buf[i] = (uint8_t) (data & 0x00FF);
		buf[i+1] = (uint8_t) (data & 0xFF00);
	}

	return 1;
}

/*
 * INPUT: buf -- length must be be multiples of 512 bytes
 * RETURN VALUE: 1 if successful, 0 if error
 * */
uint32_t ata_read_pio28(uint32_t sector, uint32_t sec_count, uint8_t* buf) {
	int32_t i;
	uint32_t status;
	uint32_t data;
	/* Sanity check */
	if (NULL == buf || sec_count >= 0x400000 || sector > 0xFFFFFFF) {
		return 0;
	}

	while (sec_count > 0) { 

		/* Set target sector and sector count */
		outb(ATA_READ_MASTER | (ATA_MASTER_SLAVEBIT << 4) |
				((sector >> 24) & 0xF), ATA_DRIVE_SELECT);
		outb(sec_count, ATA_SECTOR_COUNT);
		outb((uint8_t)sector, ATA_LBA_LOW);
		outb((uint8_t)(sector >> 8), ATA_LBA_MID);
		outb((uint8_t)(sector >> 16), ATA_LBA_HIGH);
		outb(ATA_CMD_READ, ATA_STATUS);

		/* Read Status Register for 15 times because the
		 * drive needs some time to response */
		for (i = 0; i < 15; i++) {
			status = inb(ATA_STATUS);
			if (!(status & ATA_BSY_MASK) && (status & ATA_DRQ_MASK)) {
				if (_read_to_buf_pio28(buf)) {
					sector++;
					sec_count--;
					break;
				} else {
					return 0;
				}
			}
		}
	}

	return 1;
}

void ata_init() {
	uint32_t ata_ready;
	uint8_t buf[512];
	ata_ready = ata_identify();
	ata_read_pio28(1, 1, buf);
}
