#include <stdio.h>
#include <stdlib.h>

#include "ifx_fs.h"
#include "virtual_disk.h"

static void clear_fd_table(IFXFileSystem *fs)
{
    if (fs == NULL) {
        return;
    }

    for (int i = 0; i < IFX_MAX_OPEN_FILES; i++) {
        fs->fd_table[i].used = 0;
        fs->fd_table[i].cursor = 0;
        fs->fd_table[i].size = 0;
        fs->fd_table[i].first_block = -1;
    }
}

int ifx_init(IFXFileSystem *fs)
{
    if (fs == NULL) {
        return -1;
    }

    fs->disk = init_disk();
    fs->mounted = 0;
    clear_fd_table(fs);

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

int ifx_write_file(IFXFileSystem *fs, const char *filename, const unsigned char *data, int size)
{
    if (fs == NULL || fs->disk == NULL || !fs->mounted) {
        return -1;
    }

    return write_root_directory_file(fs->disk, filename, data, size);
}

int ifx_open_file(IFXFileSystem *fs, const char *filename)
{
    VirtualDiskFileRecord record;
    int fd = -1;

    if (fs == NULL || fs->disk == NULL || !fs->mounted) {
        return -1;
    }

    for (int i = 0; i < IFX_MAX_OPEN_FILES; i++) {
        if (!fs->fd_table[i].used) {
            fd = i;
            break;
        }
    }

    if (fd == -1) {
        return -1;
    }

    if (load_root_directory_file_record(fs->disk, filename, &record) != 0) {
        return -1;
    }

    fs->fd_table[fd].used = 1;
    fs->fd_table[fd].cursor = 0;
    fs->fd_table[fd].size = record.size;
    fs->fd_table[fd].first_block = record.first_block;

    return fd;
}

int ifx_read_file(IFXFileSystem *fs, int fd, unsigned char *buffer, int count)
{
    IFXFileDescriptor *descriptor;
    int available;
    int bytesToRead;
    int bytesRead;

    if (fs == NULL || fs->disk == NULL || !fs->mounted) {
        return -1;
    }

    if (fd < 0 || fd >= IFX_MAX_OPEN_FILES || !fs->fd_table[fd].used) {
        return -1;
    }

    if (count < 0) {
        return -1;
    }

    if (count > 0 && buffer == NULL) {
        return -1;
    }

    descriptor = &fs->fd_table[fd];

    if (descriptor->cursor >= descriptor->size || count == 0) {
        return 0;
    }

    available = descriptor->size - descriptor->cursor;
    bytesToRead = count < available ? count : available;

    bytesRead = read_root_directory_file_bytes(
        fs->disk,
        descriptor->first_block,
        descriptor->cursor,
        buffer,
        bytesToRead
    );

    if (bytesRead < 0) {
        return -1;
    }

    descriptor->cursor += bytesRead;

    return bytesRead;
}

int ifx_close_file(IFXFileSystem *fs, int fd)
{
    if (fs == NULL) {
        return -1;
    }

    if (fd < 0 || fd >= IFX_MAX_OPEN_FILES || !fs->fd_table[fd].used) {
        return -1;
    }

    fs->fd_table[fd].used = 0;
    fs->fd_table[fd].cursor = 0;
    fs->fd_table[fd].size = 0;
    fs->fd_table[fd].first_block = -1;

    return 0;
}

int ifx_close(IFXFileSystem *fs)
{
    if (fs == NULL) {
        return -1;
    }

    if (fs->disk == NULL) {
        fs->mounted = 0;
        clear_fd_table(fs);
        return 0;
    }

    if (fclose(fs->disk) != 0) {
        fs->disk = NULL;
        fs->mounted = 0;
        clear_fd_table(fs);
        return -1;
    }

    fs->disk = NULL;
    fs->mounted = 0;
    clear_fd_table(fs);

    return 0;
}
