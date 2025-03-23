#include "atapi.h"
#include "disk/fs.h"
#include "io.h"
#include "k/atapi.h"
#include "k/types.h"
#include "stdio.h"

static unsigned int port = 0;
static unsigned int drive = 0;

SCSI_packet create_packet(unsigned int block, unsigned int nb_block)
{
	SCSI_packet packet = {
		.op_code = READ_12,
		.flags_lo = 0,
		.lba_hi = (block >> 0x18) & 0xFF,
		.lba_mihi = (block >> 0x10) & 0xFF,
		.lba_milo = (block >> 0x08) & 0xFF,
		.lba_lo = (block >> 0x00) & 0xFF,
		.transfer_length_hi = (nb_block >> 0x18) & 0xFF,
		.transfer_length_mihi = (nb_block >> 0x10) & 0xFF,
		.transfer_length_milo = (nb_block >> 0x08) & 0xFF,
		.transfer_length_lo = (nb_block >> 0x00) & 0xFF,
		.flags_hi = 0,
		.control = 0,
	};
	return packet;
}

static void wait_400ns(void)
{
	inb(0x1);
	inb(0x1);
	inb(0x1);
	inb(0x1);
}

void wait_long(void)
{
	for (int i = 0; i < 10000; i++)
		wait_400ns();
}

int wait_status(int set, int clear)
{
	int cur;
	do {
		cur = inb(ATA_REG_STATUS(port));
		if (cur & ERR)
			return 0;
	} while ((set && !(cur & set)) || (cur && (cur & clear)));
	return 1;
}

void reset(void)
{
	// Send reset to the first drive
	outb(PRIMARY_DCR, SRST);
	outb(PRIMARY_DCR, INTERRUPT_DISABLE);
	// Send reset to the second drive
	outb(SECONDARY_DCR, SRST);
	outb(SECONDARY_DCR, INTERRUPT_DISABLE);
	wait_400ns();
}

int sector_wait(int value)
{
	int sector;
	int status;
	do {
		sector = inb(ATA_REG_SECTOR_COUNT(port));
		status = inb(ATA_REG_STATUS(port));
		if (status & ERR)
			return 0;
		if (sector > value)
			return 0;
	} while (sector != value);
	return 1;
}

/*
                             ==============
                             " Read Block "
                             ==============
*/

int send_scsi()
{
	if (!wait_status(0, BSY))
		return 0;
	outb(ATA_REG_FEATURES(port), 0); // No overlap / no DMA
	outb(ATA_REG_SECTOR_COUNT(port), 0); // No queuing
	outb(ATA_REG_LBA_MI(port), CD_BLOCK_SZ & 0xff);
	outb(ATA_REG_LBA_HI(port), (CD_BLOCK_SZ >> 8) & 0xff);
	outb(ATA_REG_COMMAND(port), PACKET); /* PACKET */
	if (!wait_status(DRQ, BSY))
		return 0;
	sector_wait(PACKET_AWAIT_COMMAND);
	return 1;
}

void read_data(char *buffer, unsigned int size)
{
	// Read Data
	for (unsigned int i = 0; i < size / 2; i++) {
		u16 data = inw(ATA_REG_DATA(port));
		buffer[2 * i] = data & 0xff;
		buffer[2 * i + 1] = data >> 8;
	}
	int sector = inb(ATA_REG_SECTOR_COUNT(port));
	while (sector != PACKET_COMMAND_COMPLETE)
		sector = inb(ATA_REG_SECTOR_COUNT(port));
}

int request_block(SCSI_packet *packet)
{
	if (!send_scsi())
		return 0;
	u16 *words = (u16 *)packet;
	for (int i = 0; i < PACKET_SZ / 2; i++)
		outw(ATA_REG_DATA(port), words[i]);
	return sector_wait(PACKET_DATA_TRANSMIT);
}

int read_block(unsigned int block, unsigned int nb_block, char *buffer)
{
	reset();
	outb(ATA_REG_DRIVE(port), drive);
	wait_400ns();
	wait_status(0, BSY);
	SCSI_packet read_packet = create_packet(block, nb_block);
	if (!request_block(&read_packet))
		return 0;
	wait_status(RDY, BSY);
	read_data(buffer, nb_block * CD_BLOCK_SZ);
	return 1;
}

/*
                                    =========
                                    " Setup "
                                    =========
*/

int discover_drive(unsigned int disc_port, unsigned int disc_drive)
{
	outb(ATA_REG_DRIVE(disc_port), disc_drive);
	if (!wait_status(0, BSY))
		return 0;
	wait_400ns();
	/* Look for ATAPI signature */
	static unsigned int sig[4] = { 0 };
	sig[0] = inb(disc_port + ATA_REG_SECTOR_COUNT(disc_drive));
	sig[1] = inb(disc_port + ATA_REG_LBA_LO(disc_drive));
	sig[2] = inb(disc_port + ATA_REG_LBA_MI(disc_drive));
	sig[3] = inb(disc_port + ATA_REG_LBA_HI(disc_drive));
	/* printf("Signature: %02x %02x %02x %02x\n", sig[0], sig[1], sig[2], sig[3]); */
	if (sig[0] == ATAPI_SIG_SC && sig[1] == ATAPI_SIG_LBA_LO &&
	    sig[2] == ATAPI_SIG_LBA_MI && sig[3] == ATAPI_SIG_LBA_HI) {
		port = disc_port;
		drive = disc_drive;
		printf("Drive found !\n");
		return 1;
	}
	return 0;
}

int discover_drives()
{
	reset();
	if (discover_drive(PRIMARY_REG, ATA_PORT_MASTER) ||
	    discover_drive(PRIMARY_REG, ATA_PORT_SLAVE) ||
	    discover_drive(SECONDARY_REG, ATA_PORT_MASTER) ||
	    discover_drive(SECONDARY_REG, ATA_PORT_SLAVE)) {
		return 1;
	}
	return 0;
}

int setup_atapi(void)
{
	printf("Setting up ATAPI\n");
	if (!discover_drives()) {
		printf("No drive found\n");
		return 0;
	}
	printf("ATAPI set up\n");
	return 1;
}
