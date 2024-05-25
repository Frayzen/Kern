#ifndef SERIAL_H
#define SERIAL_H

#include <stddef.h>

/*
DESCRIPTION
    write() sends up to count bytes from the buffer to the serial port COM1
RETURN VALUE
    write() returns the number of bytes sent or -1 if an error occured.
*/

int write(const char *buf, size_t count);

/*
DESCRIPTION
    read() access the first byte from the buffer of the serial port COM1
RETURN VALUE
    read() returns the first byte read or -1 if an error occured.
*/
char read(void);

void serial_init(void);

#endif /* !SERIAL_H */
