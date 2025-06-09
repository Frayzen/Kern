#include "disk.h"
#include "drivers/disk/atapi/atapi.h"
#include "drivers/config.h"
#include "drivers/disk/nvme/nvme.h"
#include "drivers/disk/nvme/nvme_io.h"

int disk_read_block(unsigned int block, unsigned int nb_block, char *buffer)
{
	if (USE_NVME)
    // ATAPI BLOCK IS 2kB but NVME is 512B
		return nvme_read(block * 4, nb_block * 4, buffer);
	else
		return atapi_read_block(block, nb_block, buffer);
}


int setup_disk()
{
	if (USE_NVME)
		nvme_init();
	else
		while (!setup_atapi())
			;
}
