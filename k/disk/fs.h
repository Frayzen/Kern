#ifndef FS_H
#define FS_H

/**
 * Setup the filesystem
 */
void setup_fs();
/**
 * Open a file
 * @param path the path to the file
 * returns the file descriptor
 */
int open(char *path);
/**
 * Read from the file
 * @param fd the file descriptor
 * @param buf the buffer to read into
 * @param len the length to read
 * returns the number of bytes read
 */
int read(int fd, char *buf, unsigned int len);
/**
 * Seek to a position in the file
 * @param fd the file descriptor
 * @param offset the offset to seek to
 * @param whence the whence to seek from
 * returns the new offset
 */
int seek(int fd, int offset, int whence);
/**
 * Close a file
 * @param fd the file descriptor
 * returns 0 on success, -1 on failure
 */
int close(int fd);

#endif /* !FS_H */
