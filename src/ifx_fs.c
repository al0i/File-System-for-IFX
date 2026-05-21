#include <stdio.h>
#include <stdlib.h>

#include "ifx_fs.h"
#include "virtual_disk.h"

int ifx_init(IFXFileSystem *fs)
{
    if (fs == NULL) {
        return -1;
    }

    fs->disk = init_disk();
    fs->mounted = 0;

    if (fs->disk == NULL) {
        return -1;
    }

    return 0;
}

int ifx_mount(IFXFileSystem *fs)
{
    if (fs == NULL || fs->disk == NULL) {
        return -1;
    }

    if (mount_or_format_virtual_disk(fs->disk) != 0) {
        return -1;
    }

    fs->mounted = 1;

    return 0;
}

int ifx_create_file(IFXFileSystem *fs, const char *filename)
{
    if (fs == NULL || fs->disk == NULL || !fs->mounted) {
        return -1;
    }

    return create_root_directory_file(fs->disk, filename);
}

int ifx_close(IFXFileSystem *fs)
{
    if (fs == NULL) {
        return -1;
    }

    if (fs->disk == NULL) {
        fs->mounted = 0;
        return 0;
    }

    if (fclose(fs->disk) != 0) {
        fs->disk = NULL;
        fs->mounted = 0;
        return -1;
    }

    fs->disk = NULL;
    fs->mounted = 0;

    return 0;
}
