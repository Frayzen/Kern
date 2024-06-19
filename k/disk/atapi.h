#ifndef ATAPI_H
#define ATAPI_H

/*
 * Detect and intialize the first ATAPI devices on the system.
 * This function should be called before any other ATAPI functions.
 */
void atapi_init(void);
void disk_update(unsigned int reg);

enum disk_state {
    DISK_STATE_AVAILABLE,
    DISK_READING,
};

#endif /* !ATAPI_H */
