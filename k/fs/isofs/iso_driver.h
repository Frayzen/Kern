#ifndef ISO_DRIVER_H
#define ISO_DRIVER_H

/**
 * Finds the file in the filesystem
 * name is the relative path to the file
 * returns a pointer to the file or NULL if not found
 * returns 0 if file system does not match, 1 otherwise
 */
#include "k/types.h"
int setup_iso(void);
/**
 * Finds the file in the filesystem
 * @param name the absolute path to the file
 * @param size the size of the file
 * returns 0 if not found, the block index of the file otherwise
 */
int find(char *name, u32 *size);

#endif /* !ISO_DRIVER_H */
