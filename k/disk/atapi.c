#include "atapi.h"
#include "io.h"
#include "k/atapi.h"
#include "memory.h"
#include "serial.h"
#include "stdio.h"

SCSI_packet create_packet(unsigned int block)
{
	SCSI_packet packet = { 0 };
	packet.lba_lo = block & 0xff;
	packet.lba_hi = (block >> 8) & 0xff;
	packet.transfer_length_hi = (CD_BLOCK_SZ >> 8) & 0xff;
	packet.transfer_length_lo = CD_BLOCK_SZ & 0xff;
    packet.op_code = READ_12;
	return packet;
}

static void wait_400ns(void)
{
	inb(PRIMARY_DCR);
	inb(PRIMARY_DCR);
	inb(PRIMARY_DCR);
	inb(PRIMARY_DCR);
}

void send_reset(unsigned int dcr)
{
	outb(dcr, INTERRUPT_DISABLE);
	outb(dcr, SRST);
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

static unsigned int rgstr = 0;
static unsigned int drive = 0;
static enum disk_state state = DISK_STATE_AVAILABLE;

int discover_drive(unsigned int reg, unsigned int port)
{
	outb(ATA_REG_DRIVE(reg), port);
	wait_400ns();
	check_error(reg);
	/* Look for ATAPI signature */
	static unsigned int sig[4] = { 0 };
	sig[0] = inb(reg + ATA_REG_SECTOR_COUNT(port));
	sig[1] = inb(reg + ATA_REG_LBA_LO(port));
	sig[2] = inb(reg + ATA_REG_LBA_MI(port));
	sig[3] = inb(reg + ATA_REG_LBA_HI(port));
	/* printf("Signature: %02x %02x %02x %02x\n", sig[0], sig[1], sig[2], sig[3]); */
	if (sig[0] == ATAPI_SIG_SC && sig[1] == ATAPI_SIG_LBA_LO &&
	    sig[2] == ATAPI_SIG_LBA_MI && sig[3] == ATAPI_SIG_LBA_HI) {
		rgstr = reg;
		drive = port;
		/* printf("Drive found at %x:%x\n", port, reg); */
		return 1;
	}
	return 0;
}

void atapi_init(void)
{
	println("Initializing ATAPI");
	//send reset command
	send_reset(PRIMARY_DCR);
	send_reset(SECONDARY_DCR);
	if (discover_drive(PRIMARY_REG, ATA_PORT_MASTER) ||
	    discover_drive(PRIMARY_REG, ATA_PORT_SLAVE) ||
	    discover_drive(SECONDARY_REG, ATA_PORT_MASTER) ||
	    discover_drive(SECONDARY_REG, ATA_PORT_SLAVE)) {
		println("ATAPI initialized");
		return;
	}
	println("No drive found, halting");
	asm volatile("hlt");
}

void busy_wait(void)
{
	while (inb(rgstr + ATA_REG_STATUS(drive)) & BSY)
		;
}

void packet_req_wait(void)
{
	int status;
	do
		status = inb(rgstr + ATA_REG_STATUS(drive));
	while (!(status & DRQ) || status & BSY);
}

void sector_wait(unsigned int value)
{
	unsigned int sector;
	do
		sector = inb(rgstr + ATA_REG_SECTOR_COUNT(drive));
	while (sector != value);
}

void send_scsi(SCSI_packet *packet)
{
	outb(ATA_REG_DRIVE(rgstr), drive); // Select drive
	busy_wait();

	// Send SCSI packet with ‘READ 12’ commmand
	outb(rgstr + ATA_REG_FEATURES(drive), 0);
	outb(rgstr + ATA_REG_SECTOR_COUNT(drive), 0);
	outb(rgstr + ATA_REG_LBA_MI(drive), CD_BLOCK_SZ & 0xff);
	outb(rgstr + ATA_REG_LBA_HI(drive), (CD_BLOCK_SZ >> 8) & 0xff);
	outb(rgstr + ATA_REG_COMMAND(drive), PACKET);
	packet_req_wait();

	// Send Data
	char *bytes = (char *)packet;
	for (int i = 0; i < PACKET_SZ; i++)
		outw(rgstr + ATA_REG_DATA(drive), bytes[i]);
}

void read_data(void)
{
	// Read Data
	for (int i = 0; i < CD_BLOCK_SZ * 2; i++)
		inw(rgstr + ATA_REG_DATA(drive));
	sector_wait(PACKET_COMMAND_COMPLETE);
}

#define REG_TO_STR(reg) ((reg == PRIMARY_REG) ? "PRIMARY" : "SECONDARY")
void disk_update(unsigned int reg)
{
	if (reg != rgstr) {
		printf("Disk update on non-current drive (got %s but cur is %s)\n",
		       REG_TO_STR(reg), REG_TO_STR(rgstr));
		return;
	}
	if (state == DISK_STATE_AVAILABLE) {
		println("Sending read command");
		state = DISK_READING;
        SCSI_packet read_packet = create_packet(0);
		send_scsi(&read_packet);
	} else {
		println("Reading data");
		state = DISK_STATE_AVAILABLE;
		read_data();
	}
}
