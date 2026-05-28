#ifndef VIRTUAL_DISK_H
#define VIRTUAL_DISK_H

#include <stdio.h>

typedef struct {
    int size;
    int first_block;
} VirtualDiskFileRecord;

FILE* init_disk(void);
int mount_or_format_virtual_disk(FILE *disk);
int create_root_directory_file(FILE *disk, const char *filename);
int write_root_directory_file(FILE *disk, const char *filename, const unsigned char *data, int size);
int load_root_directory_file_record(FILE *disk, const char *filename, VirtualDiskFileRecord *record);
int read_root_directory_file_bytes(FILE *disk, int firstBlock, int offset, unsigned char *buffer, int count);

#endif
