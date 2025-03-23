#ifndef ATAPI_H
#define ATAPI_H

/*
 * Detect and intialize the first ATAPI devices on the system.
 * This function should be called before any other ATAPI functions.
 * Returns 1 if the ATAPI device was found, 0 otherwise.
 */
int setup_atapi(void);

/*
 * Read a block from the ATAPI device.
 * Returns 1 if the block was read successfully, 0 otherwise.
 */
int read_block(unsigned int block, unsigned int nb_block, char *buffer);

#endif /* !ATAPI_H */
