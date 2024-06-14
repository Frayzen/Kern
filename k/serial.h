#ifndef SERIAL_H
#define SERIAL_H

#include <stddef.h>
#define print(String) write(String, sizeof(String))
#define printchar(Char) writechar(Char)
#define println(String) print(String "\r\n")

/**
 * @brief Write a string to the serial port
 * @param buf: the string to write
 * @param count: the number of characters to write
 * @return the number of characters written
*/
int write(const char *buf, size_t count);

/**
 * @brief Write a character to the serial port
 * @param buf: the character to write
 * @return 1 if the character was written, 0 otherwise
*/
int writechar(const char buf);

/*
 * @brief Read a character from the serial property
 * @return the character read
*/
char read(void);

/**
 * @brief Initialize the serial port
 */
void serial_init(void);

#endif /* !SERIAL_H */
