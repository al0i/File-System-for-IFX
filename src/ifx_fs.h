#ifndef IFX_FS_H
#define IFX_FS_H

#include <stdio.h>

typedef struct {
    FILE *disk;
    int mounted;
} IFXFileSystem;

int ifx_init(IFXFileSystem *fs);
int ifx_mount(IFXFileSystem *fs);
int ifx_create_file(IFXFileSystem *fs, const char *filename);
int ifx_close(IFXFileSystem *fs);

#endif
