#ifndef SERIAL_H
#define SERIAL_H

#define IRQ_MASTER_OFFSET 0x40
#define IRQ_SLAVE_OFFSET 0x48
#define IRQ_LIMIT 0x10

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
char read_serial(void);

/**
 * @brief Initialize the serial port
 */
void serial_init(void);

/**
 * @brief Print a string to the serial property
 * @param String: the string to print
 */
void print_uint(unsigned int val, int size);

/**
 * @brief Print a character to the serial property
 * @param Char: the character to print
 */
void print_hex(unsigned int i);

#endif /* !SERIAL_H */
