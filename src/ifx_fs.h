#ifndef IFX_FS_H
#define IFX_FS_H

#include <stdio.h>

#define IFX_MAX_OPEN_FILES 8

typedef struct {
    int used;
    int cursor;
    int size;
    int first_block;
} IFXFileDescriptor;

typedef struct {
    FILE *disk;
    int mounted;
    IFXFileDescriptor fd_table[IFX_MAX_OPEN_FILES];
} IFXFileSystem;

int ifx_init(IFXFileSystem *fs);
int ifx_mount(IFXFileSystem *fs);
int ifx_create_file(IFXFileSystem *fs, const char *filename);
int ifx_write_file(IFXFileSystem *fs, const char *filename, const unsigned char *data, int size);
int ifx_open_file(IFXFileSystem *fs, const char *filename);
int ifx_read_file(IFXFileSystem *fs, int fd, unsigned char *buffer, int count);
int ifx_close_file(IFXFileSystem *fs, int fd);
int ifx_close(IFXFileSystem *fs);

#endif
