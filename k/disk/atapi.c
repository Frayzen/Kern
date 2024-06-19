#include "atapi.h"
#include "io.h"
#include "k/atapi.h"
#include "serial.h"
#include "stdio.h"

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

int discover_drive(unsigned int reg, unsigned int port)
{
	outb(ATA_REG_DRIVE(reg), port);
	wait_400ns();
	check_error(reg);
	unsigned int sig[4] = { 0 };
	/* Look for ATAPI signature */
	sig[0] = inb(ATA_REG_SECTOR_COUNT(port));
	sig[1] = inb(ATA_REG_LBA_LO(port));
	sig[2] = inb(ATA_REG_LBA_MI(port));
	sig[3] = inb(ATA_REG_LBA_HI(port));
	if (sig[0] == ATAPI_SIG_SC && sig[1] == ATAPI_SIG_LBA_LO &&
	    sig[2] == ATAPI_SIG_LBA_MI && sig[3] == ATAPI_SIG_LBA_HI) {
		rgstr = reg;
		drive = port;
		printf("Drive found at %d:%d\n", port, reg);
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
	if (!discover_drive(PRIMARY_REG, ATA_PORT_MASTER) &&
	    !discover_drive(PRIMARY_REG, ATA_PORT_SLAVE) &&
	    !discover_drive(SECONDARY_REG, ATA_PORT_MASTER) &&
	    !discover_drive(SECONDARY_REG, ATA_PORT_SLAVE)) {
		println("No drive found, halting");
		asm volatile("hlt");
		return;
	}
    println("ATAPI initialized");
}
