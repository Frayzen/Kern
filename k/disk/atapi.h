#ifndef ATAPI_H
#define ATAPI_H

/*
 * Detect and intialize the first ATAPI devices on the system.
 * This function should be called before any other ATAPI functions.
 */
void atapi_init(void);

#endif /* !ATAPI_H */
