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

/* Read a sector (512 bytes) to given buffer
 * RETURN VALUE: 1 if successful, 0 if error
 * */
uint32_t _read_to_buf_pio28(uint8_t* buf) {
	uint32_t i;

	if (NULL == buf) {
		return 0;
	}

	/* Read the whole 512-byte sector */
	for (i = 0; i < ATA_SECTOR_SIZE/2; i++) {
		((uint16_t*)buf)[i] = inw(ATA_DATA);
	}

	return 1;
}

/*
 * INPUT: buf -- length must be be multiples of 512 bytes
 * RETURN VALUE: 1 if successful, 0 if error
 * */
uint32_t ata_read_pio28(uint32_t sector, uint32_t sec_count, uint8_t* buf) {
	uint32_t status;
	/* Sanity check */
	if (NULL == buf || sector > 0xFFFFFFF) {
		return 0;
	}

	while (sec_count > 0) {
		/* Set target sector and sector count */
		outb(ATA_RW_MASTER | (ATA_MASTER_SLAVEBIT << 4) |
				((sector >> 24) & 0xF), ATA_DRIVE_SELECT);
		outb(1, ATA_SECTOR_COUNT);
		outb((uint8_t)sector, ATA_LBA_LOW);
		outb((uint8_t)(sector >> 8), ATA_LBA_MID);
		outb((uint8_t)(sector >> 16), ATA_LBA_HIGH);
		outb(ATA_CMD_READ, ATA_STATUS);

		while (1) {
			status = inb(ATA_STATUS);
			if (!(status & ATA_BSY_MASK) && (status & ATA_DRQ_MASK)) {
				if (_read_to_buf_pio28(buf)) {
					buf += ATA_SECTOR_SIZE;
					sector++;
					sec_count--;
					break;
				} else {
					return 0;	/* error */
				}
			}
		}
	}

	return 1;
}

void ata_cache_flush() {
	outb(ATA_MASTER_DRIVE, ATA_DRIVE_SELECT);
	outb(ATA_CMD_CACHE_FLUSH, ATA_STATUS);
	while (inb(ATA_STATUS) & ATA_BSY_MASK) {
		/* Wait BSY to be cleared */
	}
}

/*
 * INPUT: buf -- length must be be multiples of 512 bytes
 * */
uint32_t ata_write_pio28(uint32_t sector, uint32_t sec_count, uint8_t* buf) {
	uint32_t i;
	uint32_t num_sec_written;
	uint16_t data;
	uint32_t status;
	printf("Writing to sector %d\n", sector);			// DEBUG

	/* Sanity check */
	if (NULL == buf || sector > 0xFFFFFFF) {
		return 0;
	}

	num_sec_written = 0;
	while (sec_count > num_sec_written) { 
		/* Set target sector */
		outb(ATA_RW_MASTER | (ATA_MASTER_SLAVEBIT << 4) |
				(((num_sec_written + sector) >> 24) & 0xF), ATA_DRIVE_SELECT);
		outb(1, ATA_SECTOR_COUNT);
		outb((uint8_t)(num_sec_written + sector), ATA_LBA_LOW);
		outb((uint8_t)((num_sec_written + sector) >> 8), ATA_LBA_MID);
		outb((uint8_t)((num_sec_written + sector) >> 16), ATA_LBA_HIGH);
		outb(ATA_CMD_WRITE, ATA_STATUS);

		printf("Waiting for DRQ\n");						// DEBUG

		while(!(inb(ATA_STATUS) & ATA_DRQ_MASK)) {
			/* Wait until DRQ to be set */
		}
		printf("DRQ set\n");								// DEBUG

		for (i = 0; i < ATA_SECTOR_SIZE/2; i++) {
			outw(((uint16_t*)buf)[i + ATA_SECTOR_SIZE * num_sec_written / 2],
				ATA_DATA);
			ata_cache_flush();
		}
		num_sec_written++;
	}

	return 1;
}
