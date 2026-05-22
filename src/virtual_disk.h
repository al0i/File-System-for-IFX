#ifndef VIRTUAL_DISK_H
#define VIRTUAL_DISK_H

#include <stdio.h>

FILE* init_disk(void);
int mount_or_format_virtual_disk(FILE *disk);
int create_root_directory_file(FILE *disk, const char *filename);
int write_root_directory_file(FILE *disk, const char *filename, const unsigned char *data, int size);

#endif
