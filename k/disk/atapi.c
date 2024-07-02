#include "atapi.h"
#include "io.h"
#include "k/atapi.h"
#include "k/types.h"
#include "serial.h"
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

// Control register defines
#define CONTROL 0x206
static void wait_400ns(void)
{
	inb(port + CONTROL);
	inb(port + CONTROL);
	inb(port + CONTROL);
	inb(port + CONTROL);
}

void check_error(unsigned int reg)
{
	unsigned int status = inb(ATA_REG_STATUS(reg));
	if (status & ERR) {
		printf("Error: %d\n", status);
		while (1)
			;
	}
}

int discover_drive(unsigned int disc_port, unsigned int disc_drive)
{
	outb(ATA_REG_DRIVE(disc_port), disc_drive);
	wait_400ns();
	check_error(disc_port);
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
void busy_wait(void)
{
	while (inb(ATA_REG_STATUS(port)) & BSY)
		;
}

void wait_packet_req(void)
{
	int status;
	do {
		status = inb(ATA_REG_COMMAND(port));
	} while (!(status & DRQ) || status & BSY);
}

void sector_wait(unsigned int value)
{
	unsigned int sector;
	do {
		sector = inb(ATA_REG_SECTOR_COUNT(port));
	} while (sector != value);
}

void send_scsi(SCSI_packet *packet)
{
	busy_wait();
	outb(ATA_REG_FEATURES(port), 0); // No overlap / no DMA
	outb(ATA_REG_SECTOR_COUNT(port), 0); // No queuing
	outb(ATA_REG_LBA_MI(port), CD_BLOCK_SZ & 0xff);
	outb(ATA_REG_LBA_HI(port), (CD_BLOCK_SZ >> 8) & 0xff);
	outb(ATA_REG_COMMAND(port), PACKET); /* PACKET */
	printf("Waiting for request\n");
	/* wait_packet_req(); */
	sector_wait(PACKET_AWAIT_COMMAND);
	// Send Data
	u16 *bytes = (u16 *)packet;
	for (int i = 0; i < PACKET_SZ / 2; i++)
		outw(ATA_REG_DATA(port), bytes[i]);
	wait_400ns();
	/* read(); */
	printf("Waiting for data\n");
	int sector = inb(ATA_REG_SECTOR_COUNT(port));
	printf("Sector aft: %d\n", sector);
	sector_wait(PACKET_DATA_TRANSMIT);
}

void read_data(char *buffer, unsigned int nb_block)
{
	// Read Data
	for (unsigned int i = 0; i < nb_block * CD_BLOCK_SZ / 2; i++) {
		u16 data = inw(ATA_REG_DATA(port));
		buffer[2 * i] = data & 0xff;
		buffer[2 * i + 1] = data >> 8;
	}
	int sector = inb(ATA_REG_SECTOR_COUNT(port));
	while (sector != PACKET_COMMAND_COMPLETE) {
		inb(ATA_REG_DATA(port));
		sector = inb(ATA_REG_SECTOR_COUNT(port));
	}
}

int discover_drives()
{
	// Send reset to the first drive
	outb(PRIMARY_DCR, INTERRUPT_DISABLE);
	outb(PRIMARY_DCR, SRST);
	// Send reset to the second drive
	outb(SECONDARY_DCR, INTERRUPT_DISABLE);
	outb(SECONDARY_DCR, SRST);
	if (discover_drive(PRIMARY_REG, ATA_PORT_MASTER) ||
	    discover_drive(PRIMARY_REG, ATA_PORT_SLAVE) ||
	    discover_drive(SECONDARY_REG, ATA_PORT_MASTER) ||
	    discover_drive(SECONDARY_REG, ATA_PORT_SLAVE)) {
		return 1;
	}
	return 0;
}

void read_block(unsigned int block, unsigned int nb_block, char *buffer)
{
	SCSI_packet read_packet = create_packet(block, nb_block);
	send_scsi(&read_packet);
	read_data(buffer, nb_block);
}

void setup_atapi(void)
{
	println("Initializing ATAPI");
	if (!discover_drives()) {
		println("No drive found, halting");
		asm volatile("hlt");
	}
	println("ATAPI initialized");
	return;
}
