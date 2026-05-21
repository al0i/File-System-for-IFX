#include <stdio.h>
#include <stdlib.h>

#include "virtual_disk.c"

int main()
{
    FILE *disk = init_disk();

    if (disk == NULL) {
        return -1;
    }

    if (mount_or_format_virtual_disk(disk) != 0)
    {
        fclose(disk);
        return -1;
    }

    fclose(disk);

    return 0;
}
