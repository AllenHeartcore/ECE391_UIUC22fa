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


void ata_init() {
	uint32_t ata_ready;
	ata_ready = ata_identify();
}
