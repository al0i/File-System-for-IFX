#include <stdio.h>
#include <stdlib.h>

#include "ifx_fs.h"

int main()
{
    IFXFileSystem fs;
    const unsigned char message[] = "IFX";
    unsigned char buffer[4];
    int fd;
    int bytesRead;

    if (ifx_init(&fs) != 0) {
        return -1;
    }

    if (ifx_mount(&fs) != 0) {
        ifx_close(&fs);
        return -1;
    }

    ifx_create_file(&fs, "DEMO.TXT");

    if (ifx_write_file(&fs, "DEMO.TXT", message, 3) != 0) {
        ifx_close(&fs);
        return -1;
    }

    fd = ifx_open_file(&fs, "DEMO.TXT");

    if (fd < 0) {
        ifx_close(&fs);
        return -1;
    }

    bytesRead = ifx_read_file(&fs, fd, buffer, 2);

    if (bytesRead != 2) {
        ifx_close_file(&fs, fd);
        ifx_close(&fs);
        return -1;
    }

    bytesRead = ifx_read_file(&fs, fd, buffer + 2, 2);

    if (bytesRead != 1) {
        ifx_close_file(&fs, fd);
        ifx_close(&fs);
        return -1;
    }

    buffer[3] = '\0';
    printf("Arquivo lido: %s\n", buffer);

    if (ifx_close_file(&fs, fd) != 0) {
        ifx_close(&fs);
        return -1;
    }

    if (ifx_close(&fs) != 0) {
        return -1;
    }

    return 0;
}
